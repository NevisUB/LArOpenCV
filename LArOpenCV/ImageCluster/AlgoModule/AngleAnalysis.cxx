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
    
    _angle_cut = pset.get<int>("AngleCut");

    _CalcAngle.Configure(pset.get<Config_t>("CalcAngle"));

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
    for(size_t plane=0;plane<(size_t)_nplanes;++plane) {
      Register(new data::ParticleClusterArray);
    }
  }

  void AngleAnalysis::ClearEvent() {
    _vtxid = kINVALID_INT;
    ClearVertex();
  }
  
  void AngleAnalysis::ClearVertex(){
    
    _angle_0_v.clear();   
    _angle_0_v.resize(3,-1.0*kINVALID_DOUBLE);
    
    _angle_1_v.clear();
    _angle_1_v.resize(3,-1.0*kINVALID_DOUBLE);
    
    _anglediff_v.clear();
    _anglediff_v.resize(3,-1.0*kINVALID_DOUBLE);
    
    _straightness = 0;

    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

    _nparticles         = kINVALID_INT;
    _anglediff          = kINVALID_DOUBLE;
    _anglediff_straight = kINVALID_DOUBLE;
    _anglediff_max      = kINVALID_DOUBLE;
  }
  
  bool AngleAnalysis::_PostProcess_() const
  { return true; }

  void AngleAnalysis::_Process_() {
    LAROCV_INFO() << "start" << std::endl;
    ClearEvent();
    
    if(NextEvent()) _roid =0;
    
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();
    // Get the Ass Man
    auto& ass_man = AssManager();
    
    // Get the vertex from pervious modules
    std::vector<const data::Vertex3D*> vertex_data_v;
    
    if(_combined_vertex_analysis_algo_id!=kINVALID_ALGO_ID) {
      const auto& input_vertex_data_v
	= AlgoData<data::Vertex3DArray>(_combined_vertex_analysis_algo_id,0).as_vector();
      for(const auto& vtx : input_vertex_data_v)
	vertex_data_v.push_back(&vtx);
    }
    
    _vtxid = -1;
    
    
    for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
      
      ClearVertex();

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
	LAROCV_DEBUG() << "@plane=" << plane << std::endl;
	const auto& circle_vertex = vertex3d->cvtx2d_v.at(plane);

	// Input algo data
	
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	double angle0 = -1.0*kINVALID_DOUBLE;
	double angle1 = -1.0*kINVALID_DOUBLE;	
	int pid = 0 ;
	for(auto par_id : par_ass_id_v) {
	  LAROCV_DEBUG() << "@par_id=" << par_id << std::endl;
	  auto par = particle_v.at(par_id)._par_v.at(plane);
	  if (par._ctor.empty()) continue;

	  double angle;
	  
	  _CalcAngle.AngleWithCircleResolution( angle, img_v.at(plane), par._ctor, circle_vertex );
	  
	  if(pid == 0){
	    angle0 = angle;
	    _angle_0_v.at(plane) = angle; 
	  }
	  if(pid == 1){
	      angle1 = angle;
	      _angle_1_v.at(plane) = angle; 
	      par._adiff = std::abs(angle1 - angle0);
	  }
	  
	  par._angle = angle;
	  //par._par_angle = par_angle;
	  //par._circle = circle;
	  //par._angle_scan_end_point = end_point;
	
	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  AssociateOne(particle_v.at(par_id),this_par_data.as_vector().back());
	  
	  ++pid;
	}

	double anglediff = -1.0*kINVALID_DOUBLE; 
	if (angle0 > -1.0*kINVALID_DOUBLE && angle1 > -1.0*kINVALID_DOUBLE) {
	  anglediff = std::abs(angle0-angle1);
	  if (anglediff>180) anglediff = 360-anglediff;
	  _anglediff = anglediff;
	  _anglediff_v.at(plane) = anglediff;
	}
	if (anglediff > _angle_cut) _straightness++;

      }
      _anglediff_straight = _anglediff; 
      if(_anglediff < 10 && _straightness > 0) 
	_anglediff_straight = *(std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v))); 

      _anglediff_max = *(std::max_element(std::begin(_anglediff_v), std::end(_anglediff_v))); 
      _tree->Fill();

    }//loop of vertex   


    _roid += 1;
    LAROCV_INFO() << "end" << std::endl;
  }
}
#endif
