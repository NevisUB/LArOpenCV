#ifndef __DQDSANALYSIS_CXX__
#define __DQDSANALYSIS_CXX__

#include "dQdsAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/SpectrumAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

namespace larocv {

  /// Global larocv::dQdsAnalysisFactory to register AlgoFactory
  static dQdsAnalysisFactory __global_dQdsAnalysisFactory__;
  
  void dQdsAnalysis::_Configure_(const Config_t &pset)
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

    _angle_analysis_algo_id = kINVALID_ALGO_ID;
    // Input from AngleAnalysisAlgo
    auto angle_analysis_algo_name = pset.get<std::string>("AngleAnalysisAlgo","");
    if (!angle_analysis_algo_name.empty()) {
      _angle_analysis_algo_id = this->ID(angle_analysis_algo_name);
      if (_angle_analysis_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ParticleCluster name is INVALID!");
    }
    
    //
    // Configure AtomicAnalysis
    //
    _AtomicAnalysis.Configure(pset.get<Config_t>("AtomicAnalysis"));
    //
    
    _dqds_scan_thre = pset.get<float>("dQdsScanThre");
    
    _tree = new TTree("dQdSAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"        , &_roid      , "roid/I");
    _tree->Branch("vtxid"       , &_vtxid     , "vtxid/I");
    _tree->Branch("x"           , &_x         , "x/D");
    _tree->Branch("y"           , &_y         , "y/D");
    _tree->Branch("z"           , &_z         , "z/D");
    //_tree->Branch("dqds_vvv"    ,&_dqds_vvv          );
    _tree->Branch("dqds_diff_v" , &_dqds_diff_v       );
    _tree->Branch("dqds_ratio_v" ,&_dqds_ratio_v       );
    //_tree->Branch("dqds_0" ,&_dqds_0_v       );
    //_tree->Branch("dqds_1" ,&_dqds_1_v       );

    _roid = 0;
    
    // Register 3 particle arrays, 1 per plane
    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }
  
  void dQdsAnalysis::Clear(){
    _dqds_diff_v.clear();
    _dqds_diff_v.resize(3,-9999);
    _dqds_0_v.clear();
    _dqds_0_v.resize(3);
    _dqds_1_v.clear();
    _dqds_1_v.resize(3);
    _dqds_ratio_v.clear();
    _dqds_ratio_v.resize(3,-9999);

  }
  
  bool dQdsAnalysis::_PostProcess_() const
  { return true; }

  void dQdsAnalysis::_Process_() {
    
    if(NextEvent()) _roid =0;
    
    bool show =false;
    
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();
   // Get the Ass Manager
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

      _vtxid  =vertex_id;
      
      if(show)std::cout<<"====>>>>dQds Analysis Starts<<<<====="<<std::endl;
      
      const auto& vertex3d = vertex_data_v[vertex_id];
      _x = vertex3d->x;
      _y = vertex3d->y;
      _z = vertex3d->z;
      
      for(size_t plane =0; plane <=2; ++plane){
	if(show)std::cout<<"====>>>>Plane "<<plane<<"<<<<====="<<std::endl;
		
	// Input algo data
	const auto& par_data = AlgoData<data::ParticleClusterArray>(_angle_analysis_algo_id,plane);
	auto par_ass_id_v = ass_man.GetManyAss(*vertex3d,par_data.ID());
	if(par_ass_id_v.size()==0) continue;
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	float dqds_mean_0 =-9999.0;
	float dqds_mean_1 =-9999.0;
	int pid = 0;
	for(auto par_id : par_ass_id_v) {
	  if(show)std::cout<<"Particle ID is"<<par_id<<std::endl;
	  auto par = par_data.as_vector().at(par_id);
	  cv::Mat masked_ctor;
	  masked_ctor = MaskImage(img_v[plane],par._ctor,0,false); 	

	  geo2d::Circle<float> circle;
	  
	  circle = par._circle;

	  masked_ctor = MaskImage(masked_ctor,circle,0,false); 	
	  //masked_ctor = Threshold(masked_ctor, 10, 255);
	  
	  if ( FindNonZero(masked_ctor).size() <2 ) continue;
	  
	  data::AtomicContour thisatom;
	  
	  thisatom = FindNonZero(masked_ctor);
	  
	  auto this_pca = CalcPCA(thisatom);
	  auto start_point = circle.center;
	  geo2d::Vector<float> end_point;
	  
	  end_point = par._end_point;
	  
	  if (int(par._end_point.x == -9999)){
	    if(show)std::cout<<par._end_point.x<<std::endl;
	    FindEdge(thisatom, start_point, end_point);
	  }
	  
	  std::vector<float> this_dqds;
	  this_dqds.clear();
	  
	  this_dqds = _AtomicAnalysis.AtomdQdX(masked_ctor, thisatom, this_pca, start_point, end_point);
	  
	  size_t dqds_size = this_dqds.size();
	  
	  if (this_dqds.size()>=4){ 
	    float mean_before = 0;
	    float mean_after  = 0;
	    for (size_t idx = 0; idx < this_dqds.size()-4; ++idx){
	      mean_before = (this_dqds[idx]+this_dqds[idx+1]+this_dqds[idx+2])/3;
	      if(show)std::cout<<"mean_before, idx: "<< idx <<" | "<< idx+1<<" | "<<idx+2<<std::endl;
	      if(show)std::cout<<"dqds_before, val: "<< this_dqds[idx]<<" | "<<this_dqds[idx+1]<<" | "<<this_dqds[idx+2]<<std::endl;
	      if(show)std::cout<<"before >>>>>>>>>> "<<mean_before<<std::endl;
	      
	      mean_after  = (this_dqds[idx+1]+this_dqds[idx+2]+this_dqds[idx+3])/3;
	      if(show)std::cout<<"mean_after, idx: "<< idx+1 <<" | "<< idx+2<<" | "<<idx+3<<std::endl;
	      if(show)std::cout<<"dqds_after, val: "<< this_dqds[idx+1]<<" | "<<this_dqds[idx+2]<<" | "<<this_dqds[idx+3]<<std::endl;
	      if(show)std::cout<<"after >>>>>>>>>> "<<mean_after<<std::endl;
	      
	      if (std::abs(mean_before- mean_after)>_dqds_scan_thre) {
		dqds_size = idx+3;
		break;
	      }
	    }
	  }

	  std::vector<float> chosen_dqds;
	  chosen_dqds.clear();
	  chosen_dqds.resize(dqds_size);
	  
	  for(size_t idx = 0; idx< chosen_dqds.size(); ++idx){
	    chosen_dqds[idx] = this_dqds[idx];
	  }

	  par._vertex_dqds = chosen_dqds;
		  
	  par._dqds_mean = VectorMean(chosen_dqds);
	  
	  if(show)std::cout<<"  this dqds size is "<<this_dqds.size()<<std::endl;
	  if(show)std::cout<<"chosen dqds size is "<<chosen_dqds.size()<<std::endl;
	  	  
	  if(pid == 0){
	    dqds_mean_0 = par._dqds_mean;
	    _dqds_0_v[plane] = chosen_dqds;
	  }
	  if(pid == 1){
	    dqds_mean_1 = par._dqds_mean;
	    _dqds_1_v[plane] = chosen_dqds;
	  }
	  /*
	    AtomdQdX(const cv::Mat& img, 
	             const data::AtomicContour& atom,
	             const geo2d::Line<float>& pca,
	             const geo2d::Vector<float>& start,
	             const geo2d::Vector<float>& end) const;
	  */
	  
	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  ++pid;
	}

	_dqds_diff_v[plane] = std::abs(dqds_mean_0 - dqds_mean_1);
	
	if (show)if(show)std::cout<<"Plane "<<plane<<" dqds diff is "<<_dqds_diff_v[plane]<<std::endl;

	_dqds_ratio_v[plane] = (dqds_mean_0/dqds_mean_1 <= 1 ? dqds_mean_0/dqds_mean_1 : dqds_mean_1/dqds_mean_0 );
	
      }
      _tree->Fill();
    }//loop of vertex
    _roid += 1;
  }
}
#endif
