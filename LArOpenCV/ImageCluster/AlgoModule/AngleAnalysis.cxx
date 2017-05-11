#ifndef __ANGLEANALYSIS_CXX__
#define __ANGLEANALYSIS_CXX__

#include "AngleAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#define PI 3.1415926
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
    
    _pixels_number = pset.get<int>("PixelsNumber");
    _angle_cut = pset.get<int>("AngleCut");
    
    _tree = new TTree("AngleAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"       , &_roid      , "roid/I");
    _tree->Branch("vtxid"      , &_vtxid     , "vtxid/I");
    _tree->Branch("x"          , &_x         , "x/D");
    _tree->Branch("y"          , &_y         , "y/D");
    _tree->Branch("z"          , &_z         , "z/D");
    _tree->Branch("nparticles" , &_nparticles, "nparticles/I");
    _tree->Branch("straightness" , &_straightness, "straightness/I");
    _tree->Branch("anglediff" , &_anglediff);
    _tree->Branch("anglediff_v" , &_anglediff_v);

    _roid = 0;
    
    // Register 3 particle arrays, 1 per plane
    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }

  void AngleAnalysis::Clear(){
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
    
    for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
      
      Clear();
      
      _vtxid = vertex_id;
      bool show = false;
      //if(show)std::cout<<"====>>>>Angle Analysis Starts<<<<====="<<std::endl;
      if(show)std::cout<<"====>>>>Angle Analysis Starts<<<<====="<<std::endl;
      if(show)std::cout<<"============================================>>>>>>>>vertex id  "<<vertex_id<<std::endl;
      const auto& vertex3d = vertex_data_v[vertex_id];
      
      _x = vertex3d->x;
      _y = vertex3d->y;
      _z = vertex3d->z;
      
      for(size_t plane =0; plane <=2; ++plane){
	if(show)std::cout<<"====>>>>Plane "<<plane<<"<<<<====="<<std::endl;
	const auto& circle_vertex = vertex3d->cvtx2d_v.at(plane);
	const auto& xs_pts = circle_vertex.xs_v;
	//if(show)std::cout<<"[from vertex scan] size of xs"<<xs_pts.size()<<std::endl;	
	
	// Input algo data
	const auto& par_data = AlgoData<data::ParticleClusterArray>(_combined_vertex_analysis_algo_id,plane+1);
	auto par_ass_id_v = ass_man.GetManyAss(*vertex3d,par_data.ID());
	_nparticles = par_ass_id_v.size();
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	double angle0 =-9999.0;
	double angle1 =-9999.0;
	int pid = 0 ;
	for(auto par_id : par_ass_id_v) {
	  if(show)std::cout<<">>>Particle ID "<<par_id<<std::endl;
	  auto par = par_data.as_vector().at(par_id);
	  cv::Mat masked_ctor_start;
	  masked_ctor_start = MaskImage(img_v[plane],par._ctor,0,false); 	
	  //masked_ctor = MaskImage(masked_ctor,circle,0,false); 	
	  masked_ctor_start = Threshold(masked_ctor_start, 10, 255);
	  
	  if ( FindNonZero(masked_ctor_start).size() <2 ) continue;
	  
	  auto thisatom = FindNonZero(masked_ctor_start);
	  
	  auto start_point = circle_vertex.center;
	  
	  //geo2d::Vector<float> end_point;
	  //FindEdge(thisatom, start_point, end_point);
	  //float max_radius = pow((pow(start_point.x-end_point.x,2)+pow(start_point.y-end_point.y,2)),0.5);
	  
	  float max_radius = 100 ;
	  if(show)std::cout<<"max_radius is"<<max_radius<<std::endl;

	  int r_size = int(max_radius/1.);
	  
	  std::vector<float> radius_v;
	  radius_v.clear();
	  radius_v.resize(r_size, circle_vertex.radius);
	  
	  for(int ridx = 0; ridx < r_size ; ++ridx) radius_v[ridx] += ridx;
	  //for(int ridx = 0; ridx < r_size ; ++ridx) if(show)std::cout<<raidus_v[ridx]<<std::endl;
	  double angle;
	  double pct;
	  
	  for(int ridx = 0; ridx < r_size ; ++ridx){
	    if(show) if(show)std::cout<<"Scanning radius at "<<ridx<<" with value = "<<radius_v[ridx]<<" ";
	    geo2d::Circle<float> circle_this;
	    circle_this.radius = radius_v[ridx];
	    circle_this.center = circle_vertex.center;
	    
	    double angle_this; // Angle of this particle
	    double pct_this; // pct is actually Δpct meaning the difference of 
	    // percentages of pixel on two sides of the vertex and take the difference. 
	    // In this way we check if angle is in [0,180] or [180,360]
	    cv::Mat masked_ctor_this;
	    masked_ctor_this = MaskImage(masked_ctor_start,circle_this,0,false); 	
	    if ( FindNonZero(masked_ctor_this).size() <_pixels_number ) continue;
	    if(show)std::cout<<"size of this circle is "<<FindNonZero(masked_ctor_this).size()<<std::endl;
	    auto masked_ctor_this_img = FindNonZero(masked_ctor_this);
	    float pi_threshold = 10;
	    float supression = 0;
	    auto xs_v_this = QPointOnCircle(masked_ctor_this, circle_this, pi_threshold, supression);
	    
	    //input{[particle_image, circle(providing vtx), pct[double],angle[double]}
	    ParticleAngle(masked_ctor_this_img, circle_this, pct_this, angle_this );
	    if(show)std::cout<<" angle_this "<<angle_this;
	    if ( ridx == 0 ) continue; 
	    if ( xs_v_this.size() == 0 ) {
	      angle =  angle_this;
	      pct = pct_this;
	      break;
	    }
	    
	    float resolution_last = 360 / (2 * PI * radius_v[ridx-1]);
	    if(show)std::cout<<"resolution is "<<resolution_last<<std::endl;
	    
	    masked_ctor_start = MaskImage(img_v[plane],par._ctor,0,false); 	
	    masked_ctor_start = Threshold(masked_ctor_start, 10, 255);
	    
	    geo2d::Circle<float> circle_last;
	    circle_last.radius = radius_v[ridx-1];
	    circle_last.center = circle_vertex.center;
	    
	    double angle_last;
	    double pct_last;	    
	    cv::Mat masked_ctor_last;
	    masked_ctor_last = MaskImage(masked_ctor_start,circle_last,0,false);
	    if ( FindNonZero(masked_ctor_last).size() <_pixels_number ) continue;
	    if(show)std::cout<<"size of last circle is "<<FindNonZero(masked_ctor_last).size()<<std::endl;
	    auto masked_ctor_last_img = FindNonZero(masked_ctor_last);
	    ParticleAngle(masked_ctor_last_img, circle_last, pct_last, angle_last );
	    if(show)std::cout<<" angle_last "<<angle_last<<std::endl;
	    if (std::abs(angle_last-angle_this) > resolution_last){
	      angle =  angle_last;
	      pct = pct_last;
	      break;
	    }
	    angle =  angle_last;
	    pct = pct_last;
	  }
	  if(pid == 0) angle0 = angle;
	  if(pid == 1) angle1 = angle;
	  par._angle = angle;
	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  
	  if(show)std::cout<<"pct   is "<<pct<<std::endl;
	  if(show)std::cout<<"angle is "<<angle<<std::endl;
	  _angle_particles.push_back(angle);
	  if(show)std::cout<<"PID "<<pid<<"has angle "<<angle<<std::endl;
	  ++pid;
	}
	if(show)std::cout<<"angle0 "<<angle0<<std::endl;
	if(show)std::cout<<"angle1 "<<angle1<<std::endl;
	double anglediff = -9999; 
	if (angle0 > -9999 && angle1 > -9999) {
	  anglediff = std::abs(angle0-angle1);
	  if (anglediff>180) anglediff = 360-anglediff;
	  _anglediff = anglediff;
	  _anglediff_v[plane] = anglediff;
	}
	if (anglediff > _angle_cut) _straightness++;
	if(show)std::cout<<"angle diff "<<anglediff<<std::endl;
      }
      for (auto shit: _anglediff_v) if(show)std::cout<<"anglediff"<<shit<<std::endl;
      
      //if (_straightness>=2) _anglediff = *std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v)); 
      _tree->Fill();
    }//loop of vertex   
  }
}
#endif
