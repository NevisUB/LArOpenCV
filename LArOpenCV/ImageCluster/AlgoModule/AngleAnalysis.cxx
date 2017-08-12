#ifndef __ANGLEANALYSIS_CXX__
#define __ANGLEANALYSIS_CXX__

#include "AngleAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#define PI 3.1415926
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"

namespace larocv {

  /// Global larocv::AngleAnalysisFactory to register AlgoFactory
  static AngleAnalysisFactory __global_AngleAnalysisFactory__;
  
  void AngleAnalysis::_Configure_(const Config_t &pset)
  {
    //
    // Prepare Algo Configs
    //
    _combined_vertex_analysis_algo_id = kINVALID_ALGO_ID;
    
    // Input from CombinedVertexAnalysisAlgo
    auto combined_vertex_analysis_algo_name = pset.get<std::string>("CombinedVertexAnalysisAlgo","");
    if (!combined_vertex_analysis_algo_name.empty()) {
      _combined_vertex_analysis_algo_id = this->ID(combined_vertex_analysis_algo_name);
      if (_combined_vertex_analysis_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ParticleCluster name is INVALID!");
    }

    auto name_particle = pset.get<std::string>("ParticleProducer");
    _particle_id=kINVALID_ALGO_ID;
    if (!name_particle.empty()) {
      _particle_id = this->ID(name_particle);
      if (_particle_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }
    
    _pixels_number = pset.get<int>("PixelsNumber");
    _angle_cut = pset.get<int>("AngleCut");

    _tree = new TTree("AngleAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"       , &_roid      , "roid/I");
    _tree->Branch("vtxid"      , &_vtxid     , "vtxid/I");
    _tree->Branch("x"          , &_x         , "x/D");
    _tree->Branch("y"          , &_y         , "y/D");
    _tree->Branch("z"          , &_z         , "z/D");
    _tree->Branch("nparticles"        , &_nparticles, "nparticles/I");
    _tree->Branch("straightness"      , &_straightness, "straightness/I");
    _tree->Branch("anglediff"         , &_anglediff, "anglediff/D");
    _tree->Branch("anglediff_straight", &_anglediff_straight, "anglediff_straight/D");
    _tree->Branch("anglediff_max"     , &_anglediff_max, "anglediff_max/D");
    _tree->Branch("angle_0_v"         , &_angle_0_v);   
    _tree->Branch("angle_1_v"         , &_angle_1_v);   
    _tree->Branch("anglediff_v"       , &_anglediff_v);
    _roid = 0;
    
    // Register 3 particle arrays, 1 per plane
    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane)
      Register(new data::ParticleClusterArray);
  }

  void AngleAnalysis::Clear(){
    _angle_0_v.clear();   
    _angle_0_v.resize(3,-9999);
    _angle_1_v.clear();
    _angle_1_v.resize(3,-9999);
    _anglediff_v.clear();
    _anglediff_v.resize(3,-9999);
    _angle_particles.clear();
    _straightness = 0;
  }
  
  bool AngleAnalysis::_PostProcess_() const
  { return true; }

  void AngleAnalysis::_Process_() {
    
    if(NextEvent()) _roid =0;
    
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();
    // Get the Ass Man
    auto& ass_man = AssManager();
    
    // Get the vertex from pervious modules
    std::vector<const data::Vertex3D*> vertex_data_v;
    
    if(_combined_vertex_analysis_algo_id!=kINVALID_ALGO_ID) {
      const auto& input_vertex_data_v  = AlgoData<data::Vertex3DArray>(_combined_vertex_analysis_algo_id,0).as_vector();
      for(const auto& vtx : input_vertex_data_v)
	vertex_data_v.push_back(&vtx);
    }

    _vtxid = -1;
    for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
      
      Clear();

      LAROCV_DEBUG() << "====>>>>Angle Analysis Starts<<<<====="<<std::endl;
      LAROCV_DEBUG() << "============================================>>>>>>>>vertex id  "<<vertex_id<<std::endl;
      const auto& vertex3d = vertex_data_v[vertex_id];
      
      _x = vertex3d->x;
      _y = vertex3d->y;
      _z = vertex3d->z;
      
      const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
      const auto& particle_v = particle_arr.as_vector();
      auto par_ass_id_v = ass_man.GetManyAss(*vertex3d,particle_arr.ID());
      
      if(par_ass_id_v.size()==0) continue;
      _nparticles = par_ass_id_v.size();

      _vtxid += 1;
      
      for(size_t plane =0; plane <=2; ++plane){
	
	const auto& circle_vertex = vertex3d->cvtx2d_v.at(plane);
		
	// Input algo data
	
	//const auto& par_data = AlgoData<data::ParticleClusterArray>(_combined_vertex_analysis_algo_id,plane+1);
	//auto par_ass_id_v = ass_man.GetManyAss(*vertex3d,par_data.ID());
	
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	double angle0 = -9999;
	double angle1 = -9999;	
	int pid = 0 ;
	for(auto par_id : par_ass_id_v) {
	  auto par = particle_v.at(par_id)._par_v[plane];
	  if(par._ctor.empty()) continue;
	  cv::Mat masked_ctor_start;

	  int par_pixel_amount = par._ctor.size();

	  masked_ctor_start = MaskImage(img_v[plane],par._ctor,0,false); 	
	  //masked_ctor = MaskImage(masked_ctor,circle,0,false); 	
	  auto masked_ctor_start_img = FindNonZero(masked_ctor_start);
	  geo2d::Circle<float> par_circle;
	  par_circle.center = circle_vertex.center;
	  par_circle.radius = 100;
	  double par_angle;
	  double par_pct;
	  
	  if ( FindNonZero(masked_ctor_start).size() < 2 ) continue;

	  ParticleAngle(masked_ctor_start_img, masked_ctor_start_img, par_circle, par_pct, par_angle );


	  masked_ctor_start = Threshold(masked_ctor_start, 10, 255);
	  
	  if ( FindNonZero(masked_ctor_start).size() < 2 ) continue;
	  
	  masked_ctor_start_img = FindNonZero(masked_ctor_start);
	  
	  auto start_point = circle_vertex.center;
	  
	  //geo2d::Vector<float> end_point;
	  //FindEdge(masked_ctor_start_img, start_point, end_point);
	  //float max_radius = pow((pow(start_point.x-end_point.x,2)+pow(start_point.y-end_point.y,2)),0.5);
	  
	  float max_radius = 100 ;
		  
	  int r_size = int(max_radius/1.);
	  
	  std::vector<float> radius_v;
	  radius_v.clear();
	  radius_v.resize(r_size, circle_vertex.radius);
	  
	  for(int ridx = 0; ridx < r_size ; ++ridx) radius_v[ridx] += ridx;

	  double angle;
	  double pct;
	  geo2d::Vector<float> end_point;
	  geo2d::Vector<float> end_point_last;
	  geo2d::Circle<float> circle;
	  double angle_this; // Angle calculated with current radius
	  double angle_last; // Angle calculated with previous radius
	  geo2d::Circle<float> circle_last;
	  double pct_last;
	  
	  for(int ridx = 0; ridx < r_size ; ++ridx){
	    angle_this = -9999;
	    if(ridx == 0 && pid ==1) angle_last = -9999;
	  
	    geo2d::Circle<float> circle_this;
	    circle_this.radius = radius_v[ridx];
	    circle_this.center = circle_vertex.center;
	    
	    double pct_this;
	    cv::Mat masked_ctor_this;
	    masked_ctor_this = MaskImage(masked_ctor_start,circle_this,0,false); 	
	  	    
	    if(FindNonZero(masked_ctor_start).size()<_pixels_number) _pixels_number = FindNonZero(masked_ctor_start).size();

	    if ( ridx >0 && FindNonZero(masked_ctor_this).size() < _pixels_number ) continue;
	    
	    auto masked_ctor_this_img = FindNonZero(masked_ctor_this);
	    float pi_threshold = 10;
	    float supression = 2;
	    auto xs_v_this = QPointOnCircle(masked_ctor_this, circle_this, pi_threshold, supression);
	    
	    if (masked_ctor_this_img.size()<2) continue;
	    ParticleAngle(masked_ctor_start_img, masked_ctor_this_img, circle_this, pct_this, angle_this );
	    
	    if (angle_this == -9999) continue;
	    
	    if ( xs_v_this.size() == 0 && par_pixel_amount == masked_ctor_this_img.size() && ridx == 0) {
	      angle =  angle_this;
	      pct = pct_this;
	      circle = circle_this;
	      break;
	    }
	    
	    if ( xs_v_this.size() == 0 && par_pixel_amount == masked_ctor_this_img.size() && ridx > 1 ) {
	      angle =  angle_last;
	      pct = pct_last;
	      circle = circle_last;
	      end_point = end_point_last;
	      break;
	    }
	    
	    if ( xs_v_this.size() == 2 && ridx > 1 ) {
	      angle =  angle_last;
	      pct = pct_last;
	      circle = circle_last;
	      end_point = end_point_last;
	      break;
	    }
	    
	    if ( ridx == 0 ) continue; 
	    float resolution_last = 360 / (2 * PI * radius_v[ridx-1]);
	    	    
	    masked_ctor_start = MaskImage(img_v[plane],par._ctor,0,false); 	
	    masked_ctor_start = Threshold(masked_ctor_start, 10, 255);
	    
	    circle_last.radius = radius_v[ridx-1];
	    circle_last.center = circle_vertex.center;
	    
	    circle_this.center = circle_vertex.center;
	    
	    auto xs_v_last = QPointOnCircle(masked_ctor_this, circle_this, pi_threshold, supression);
	    if (xs_v_last.size()==1) {
	      end_point_last = xs_v_last[0]; 
	      end_point = end_point_last;
	    }
	    
	    cv::Mat masked_ctor_last;
	    masked_ctor_last = MaskImage(masked_ctor_start,circle_last,0,false);
	    if ( FindNonZero(masked_ctor_last).size() <_pixels_number ) continue;
	    
	    auto masked_ctor_last_img = FindNonZero(masked_ctor_last);
	    if (masked_ctor_last_img.size()<2) continue;
	    ParticleAngle(masked_ctor_start_img, masked_ctor_last_img, circle_last, pct_last, angle_last );
	    
	    circle = circle_last;
	    angle =  angle_last;
	    pct = pct_last;
	    /*
	    bool is_this_pos = angle_this > 0 ;
	    bool is_last_pos = angle_last > 0 ;

	    if ( is_this_pos &&  is_last_pos) continue;
	    if (!is_this_pos && !is_last_pos) continue;
	    */
	    if (std::abs(angle_last-angle_this) > resolution_last || 
		angle_last-angle_this == 0 ){
	      angle =  angle_last;
	      pct = pct_last;
	      angle_last+=1;
	      angle_last = (int)angle_last;
	      if (angle_last != 270 && angle_last !=90 )break;
	    }
	  }
	  
	  if(pid == 0) 
	    {
	      angle0 = angle;
	      _angle_0_v[plane] = angle; 
	    }
	  if(pid == 1) 
	    {
	      angle1 = angle;
	      _angle_1_v[plane] = angle; 
	      par._adiff = std::abs(angle1 - angle0);//Only when pid == 1, diff makes sense
		  
	    }
	  
	  par._angle = angle;
	  par._par_angle = par_angle;
	  par._circle = circle;
	  par._angle_scan_end_point = end_point;
	
	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  AssociateOne(particle_v.at(par_id),this_par_data.as_vector().back());
	  
	  _angle_particles.push_back(angle);
	  
	  ++pid;
	}

	double anglediff = -9999; 
	if (angle0 > -9999 && angle1 > -9999) {
	  anglediff = std::abs(angle0-angle1);
	  if (anglediff>180) anglediff = 360-anglediff;
	  _anglediff = anglediff;
	  _anglediff_v[plane] = anglediff;
	}
	if (anglediff > _angle_cut) _straightness++;

      }
      _anglediff_straight = _anglediff; 
      if(_anglediff < 10 && _straightness > 0) _anglediff_straight = *std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v)); 
      //if(_anglediff < 5 ) _anglediff_straight = *std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v)); 
      //for (auto shit: _anglediff_v) //std::cout<<"anglediff"<<shit<<std::endl;
      _anglediff_max = *std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v)); 
      //if (_straightness>=2) _anglediff = *std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v)); 
      _tree->Fill();
    }//loop of vertex   


    _roid += 1;
  }

  void AngleAnalysis::ParticleAngle(GEO2D_Contour_t ctor_origin, 
				    GEO2D_Contour_t ctor, 
				    geo2d::Circle<float> circle, 
				    double& pct, double& angle){
    
    float vtx2d_x =  circle.center.x;
    float vtx2d_y =  circle.center.y;
    
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      ctor[idx].x =  ctor[idx].x- circle.center.x;
      ctor[idx].y =  ctor[idx].y- circle.center.y;
    }
    
    auto meanx = Getx2vtxmean(ctor_origin, vtx2d_x, vtx2d_y, pct );
    auto meany = Gety2vtxmean(ctor_origin, vtx2d_x, vtx2d_y, pct );
    
    auto dir = CalcPCA(ctor).dir;

    if(meanx * dir.x < 0 || meany* dir.y <0) {dir.x *=-1; dir.y*=-1;}
    
    angle = atan2(dir.y, dir.x)*180 / M_PI;

    if (angle < 0) angle += 360;

  }

  double AngleAnalysis::Getx2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct)
  {
    double ctr_pos = 0.0;
    double ctr_neg = 0.0;
    double sum = 0;
    double mean = -999;
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      sum += ctor[idx].x - x2d;
      if (ctor[idx].x - x2d > 0) ctr_pos++;
      if (ctor[idx].x - x2d < 0) ctr_neg++;
    }
    pct = std::abs(ctr_pos - ctr_neg)/ctor.size();
    if (ctor.size()>0) mean = sum / ctor.size();
    return mean;
  }
  
  double AngleAnalysis::Gety2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct)
  {
    double ctr_pos = 0.0;
    double ctr_neg = 0.0;
    double sum = 0;
    double mean = -999;
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      sum += ctor[idx].y - y2d;
      if (ctor[idx].y - y2d > 0) ctr_pos++;
      if (ctor[idx].y - y2d < 0) ctr_neg++;
    }
    pct = std::abs(ctr_pos - ctr_neg)/ctor.size();
    if (ctor.size()>0) mean = sum / ctor.size();
    return mean;
  }  

  
}
#endif
