#ifndef __ANGLEANALYSIS_CXX__
#define __ANGLEANALYSIS_CXX__

#include "AngleAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
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
    
    // Register 3 particle arrays, 1 per plane
    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }

  bool AngleAnalysis::_PostProcess_() const
  { return true; }

  void AngleAnalysis::_Process_() {

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
      LAROCV_DEBUG()<<"====>>>>Angle Analysis Starts<<<<====="<<std::endl;
      const auto& vertex3d = vertex_data_v[vertex_id];
     
      for(size_t plane =0; plane <=2; ++plane){
	LAROCV_DEBUG()<<"====>>>>Plane "<<plane<<"<<<<====="<<std::endl;
	const auto& circle_vertex = vertex3d->cvtx2d_v.at(plane);
	const auto& xs_pts = circle_vertex.xs_v;
	LAROCV_DEBUG()<<"size of xs"<<xs_pts.size()<<std::endl;	
	geo2d::Circle<float> circle;
	circle.radius = circle_vertex.radius;
	circle.center = circle_vertex.center;
	
	// Input algo data
	const auto& par_data = AlgoData<data::ParticleClusterArray>(_combined_vertex_analysis_algo_id,plane+1);
	auto par_ass_id_v = ass_man.GetManyAss(*vertex3d,par_data.ID());
	
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	for(auto par_id : par_ass_id_v) {
	  LAROCV_DEBUG()<<"Particle ID is"<<par_id<<std::endl;
	  auto par = par_data.as_vector().at(par_id);
	  cv::Mat masked_ctor;
	  masked_ctor = MaskImage(img_v[plane],par._ctor,0,false); 	
	  masked_ctor = MaskImage(masked_ctor,circle,0,false); 	
	  masked_ctor = Threshold(masked_ctor, 10, 255);
	  
	  if ( FindNonZero(masked_ctor).size() <2 ) continue;
	  
	  double angle; // Angle of this particle
	  
	  double pct; // pct is actually Δpct meaning the difference of 
	  // percentages of pixel on two sides of the vertex and take the difference. 
	  // In this way we check if angle is in [0,180] or [180,360]
	  
	  auto masked_ctor_img = FindNonZero(masked_ctor);
	  ParticleAngle(masked_ctor_img, circle, pct, angle );

	  par._angle = angle;
	  
	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  LAROCV_DEBUG()<<"pct   is "<<pct<<std::endl;
	  LAROCV_DEBUG()<<"angle is "<<angle<<std::endl;
	}
      }
    }   
  }
}
#endif
