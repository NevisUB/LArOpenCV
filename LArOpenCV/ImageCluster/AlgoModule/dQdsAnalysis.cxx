#ifndef __DQDSANALYSIS_CXX__
#define __DQDSANALYSIS_CXX__

#include "dQdsAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/SpectrumAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"

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
	throw larbys("Given CombinedAnalysis name is INVALID!");
    }

    _angle_analysis_algo_id = kINVALID_ALGO_ID;
    // Input from AngleAnalysisAlgo
    auto angle_analysis_algo_name = pset.get<std::string>("AngleAnalysisAlgo","");
    if (!angle_analysis_algo_name.empty()) {
      _angle_analysis_algo_id = this->ID(angle_analysis_algo_name);
      if (_angle_analysis_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given Angleanalysis name is INVALID!");
    }
    _match_analysis_algo_id = kINVALID_ALGO_ID;
    // Input from AngleAnalysisAlgo
    auto match_analysis_algo_name = pset.get<std::string>("MatchAnalysisAlgo","");
    if (!match_analysis_algo_name.empty()) {
      _match_analysis_algo_id = this->ID(match_analysis_algo_name);
      if (_match_analysis_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given MatchAnalysis name is INVALID!");
    }
    
    //
    // Configure AtomicAnalysis
    //
    _AtomicAnalysis.Configure(pset.get<Config_t>("AtomicAnalysis"));
    //
    
    _dqds_scan_thre   = pset.get<float>("dQdsScanThre");
    _drop_location        = pset.get<size_t>("dQdsDropLocation");
    _window_size      = pset.get<double>("TruncateWindowSize");
    _window_size_thre = pset.get<int>("TruncateWindowSizeThre");
    _head_frac      = pset.get<double>("HeadFrac");
    _tail_frac      = pset.get<double>("TailFrac");
        
    _tree = new TTree("dQdSAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"        , &_roid      , "roid/I");
    _tree->Branch("vtxid"       , &_vtxid     , "vtxid/I");
    _tree->Branch("x"           , &_x         , "x/D");
    _tree->Branch("y"           , &_y         , "y/D");
    _tree->Branch("z"           , &_z         , "z/D");
    //_tree->Branch("dqds_vvv"    ,&_dqds_vvv          );
    _tree->Branch("dqds_diff_v"    , &_dqds_diff_v        );
    _tree->Branch("dqds_ratio_v"   , &_dqds_ratio_v       );
    _tree->Branch("dqds_0_v"       , &_dqds_0_v           );
    _tree->Branch("dqds_1_v"       , &_dqds_1_v           );
    _tree->Branch("tdqds_diff_v"   , &_t_dqds_diff_v      );
    _tree->Branch("tdqds_ratio_v"  , &_t_dqds_ratio_v     );
    _tree->Branch("tdqds_0_v"      , &_t_dqds_0_v         );
    _tree->Branch("tdqds_1_v"      , &_t_dqds_1_v         );
    _tree->Branch("dqds_diff_01"   , &_dqds_diff_01       );
    _tree->Branch("dqds_ratio_01"  , &_dqds_ratio_01      );
    _tree->Branch("dqds_diff_012"  , &_dqds_diff_012      );
    _tree->Branch("dqds_ratio_012" , &_dqds_ratio_012     );
    _tree->Branch("theta"          , &_theta              );  
    _tree->Branch("phi"            , &_phi                );
    
    
    _roid = 0;
    
    // Register 3 particle arrays, 1 per plane
    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }
  
  void dQdsAnalysis::Clear(){
    _dqds_diff_v.clear();
    _dqds_diff_v.resize(3,-9999);
    _dqds_ratio_v.clear();
    _dqds_ratio_v.resize(3,-9999);
    _dqds_0_v.clear();
    _dqds_0_v.resize(3);
    _dqds_1_v.clear();
    _dqds_1_v.resize(3);
    _t_dqds_diff_v.clear();
    _t_dqds_diff_v.resize(3,-9999);
    _t_dqds_ratio_v.clear();
    _t_dqds_ratio_v.resize(3,-9999);
    _t_dqds_0_v.clear();
    _t_dqds_0_v.resize(3);
    _t_dqds_1_v.clear();
    _t_dqds_1_v.resize(3);
  }

  std::vector<float> dQdsAnalysis::dQdsDropper (std::vector<float> input_dqds){
    
    std::vector<float> res;
    res.clear();
    res = input_dqds;

    if (input_dqds.size()>_drop_location +5){
      res.clear();
      for (size_t idx = _drop_location; idx< input_dqds.size();++idx){
	res.push_back(input_dqds[idx]);
      }
    }
    return res;
  }

  
  std::vector<float> dQdsAnalysis::PeakFinder(std::vector<float> input_dqds, double frac){
    
   bool show = false;
    
    size_t dqds_size = input_dqds.size();
    
    if (input_dqds.size()>=8){ 
      
      float mean_before = 0;
      float mean_after  = 0;
      for (size_t idx = 0; idx < input_dqds.size()-3; ++idx){
	
	mean_before = (input_dqds[idx]+input_dqds[idx+1]+input_dqds[idx+2])/3;

	if(show)std::cout<<"mean_before, idx: "<< idx <<" | "<< idx+1<<" | "<<idx+2<<std::endl;
	if(show)std::cout<<"dqds_before, val: "<< input_dqds[idx]<<" | "<<input_dqds[idx+1]<<" | "<<input_dqds[idx+2]<<std::endl;
	if(show)std::cout<<"before >>>>>>>>>> "<<mean_before<<std::endl;
	
	mean_after  = (input_dqds[idx+1]+input_dqds[idx+2]+input_dqds[idx+3])/3;
	
	if(show)std::cout<<"mean_after, idx: "<< idx+1 <<" | "<< idx+2<<" | "<<idx+3<<std::endl;
	if(show)std::cout<<"dqds_after, val: "<< input_dqds[idx+1]<<" | "<<input_dqds[idx+2]<<" | "<<input_dqds[idx+3]<<std::endl;
	if(show)std::cout<<"after >>>>>>>>>> "<<mean_after<<std::endl;
	dqds_size = idx;	
	
	if (std::abs(mean_before - mean_after)>_dqds_scan_thre && double(idx)/double(input_dqds.size())>frac) {
	  break;
	}
      }
    }
    
    std::vector<float> res;
    res.clear();
    res.resize(dqds_size);
    
    for(size_t idx = 0; idx< res.size(); ++idx){
      res[idx] = input_dqds[idx];
    }
    return res;
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
    
    const auto& threeD_data  = AlgoData<data::Info3DArray>(_match_analysis_algo_id,0);

    for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
      Clear();

      _vtxid  =vertex_id;
      if(show)std::cout<<"vertex  "<<_vtxid<<std::endl;
      if(show)if(show)std::cout<<"====>>>>dQds Analysis Starts<<<<====="<<std::endl;
      
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
	
	float dqds_mean_0 =9999.0;
	float dqds_mean_1 =9999.0;
	int pid = 0;
	for(auto par_id : par_ass_id_v) {
	  if(show)std::cout<<par_id<<std::endl;
	  if(show)std::cout<<"Particle ID is"<<par_id<<std::endl;
	    
	  auto par    = par_data.as_vector().at(par_id);

	  //auto info3d_id   = ass_man.GetOneAss(par,threeD_data.ID());
	  //if(show)std::cout<<"info id is "<<info3d_id<<std::endl;
	  //auto this_info3d = threeD_data.as_vector().at(info3d_id);
	  auto this_info3d = threeD_data.as_vector().at(par_id);

	  _theta = this_info3d.trunk_pca_theta;
	  _phi   = this_info3d.trunk_pca_phi;

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
	  
	  if (int(par._end_point.x == 9999)){
	    if(show)std::cout<<par._end_point.x<<std::endl;
	    FindEdge(thisatom, start_point, end_point);
	  }
	  
	  std::vector<float> par_dqds;
	  par_dqds.clear();
	  par_dqds = _AtomicAnalysis.AtomdQdX(masked_ctor, thisatom, this_pca, start_point, end_point);
	  
	  std::vector<float> trunk_dqds;
	  trunk_dqds.clear();
	  if (par_dqds.size() > 30){
	    for(size_t idx = 0; idx <30 ; ++idx ) trunk_dqds.push_back(par_dqds[idx]);
	  }
	  else {trunk_dqds = par_dqds;};
	  
	  std::vector<float> remove_dqds;
	  remove_dqds.clear();
	  remove_dqds = CutHeads(trunk_dqds, _head_frac, _tail_frac);
	  
	  /*
	  std::vector<float> drop_dqds;
	  drop_dqds.clear();
	  drop_dqds = dQdsDropper(this_dqds);
	  
	  std::vector<float> truncated_dqds;
	  truncated_dqds.clear();
	  if(this_dqds.size())
	    truncated_dqds = Calc_smooth_mean(drop_dqds, _window_size,_window_size_thre,_window_frac);

	  std::vector<float> chop_truncated_dqds;
	  chop_truncated_dqds.clear();
	  chop_truncated_dqds = PeakFinder(truncated_dqds,0.1);
	  */
	  par._vertex_dqds = par_dqds;
	  //par._truncated_dqds = drop_dqds;
	  par._truncated_dqds = remove_dqds;
	  par._dqds_mean = VectorMean(remove_dqds);
	  
	  if(pid == 0){
	    dqds_mean_0 = par._dqds_mean;
	    _dqds_0_v[plane] = dqds_mean_0;
	  }
	  if(pid == 1){
	    dqds_mean_1 = par._dqds_mean;
	    _dqds_1_v[plane] = dqds_mean_1;
	  }

	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  ++pid;
	}

	_dqds_diff_v[plane] = std::abs(dqds_mean_0 - dqds_mean_1);
	
	if (show)if(show)std::cout<<"Plane "<<plane<<" dqds diff is "<<_dqds_diff_v[plane]<<std::endl;

	_dqds_ratio_v[plane] = (dqds_mean_0/dqds_mean_1 <= 1 ? dqds_mean_0/dqds_mean_1 : dqds_mean_1/dqds_mean_0 );
	
      }
      _dqds_diff_01 = 9999;
      if (_dqds_diff_v[0] >= 0 )_dqds_diff_01 = _dqds_diff_v[0];
      if (_dqds_diff_v[1] > _dqds_diff_01 && _dqds_diff_v[1]>0) _dqds_diff_01 = _dqds_diff_v[1];

      _dqds_ratio_01 = _dqds_ratio_v[0];
      if (_dqds_ratio_v[1]< _dqds_ratio_01 && _dqds_ratio_v[1]>0) _dqds_ratio_01 = _dqds_ratio_v[1];
      
      _tree->Fill();
    }//loop of vertex
    _roid += 1;
  }
}
#endif
