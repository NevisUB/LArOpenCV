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

    _match_overlap_algo_id = kINVALID_ALGO_ID;
    // Input from MatchOverlap
    auto match_overlap_algo_name = pset.get<std::string>("MatchOverlapAlgo","");
    if (!match_overlap_algo_name.empty()) {
      _match_overlap_algo_id = this->ID(match_overlap_algo_name);
      if (_match_overlap_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given MatchOverlap name is INVALID!");
    }
    
    _angle_analysis_algo_id = kINVALID_ALGO_ID;
    // Input from AngleAnalysisAlgo
    auto angle_analysis_algo_name = pset.get<std::string>("AngleAnalysisAlgo","");
    if (!angle_analysis_algo_name.empty()) {
      _angle_analysis_algo_id = this->ID(angle_analysis_algo_name);
      if (_angle_analysis_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given AngleAnalysis name is INVALID!");
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
    
    _tree->Branch("dqds_0_v"       , &_dqds_0_v           );
    _tree->Branch("dqds_1_v"       , &_dqds_1_v           );
    _tree->Branch("dqds_diff_v"    , &_dqds_diff_v        );
    _tree->Branch("dqds_ratio_v"   , &_dqds_ratio_v       );
    _tree->Branch("dqds_diff_01"   , &_dqds_diff_01       );
    _tree->Branch("dqds_ratio_01"  , &_dqds_ratio_01      );

    _tree->Branch("tdqds_0_v"      , &_t_dqds_0_v         );
    _tree->Branch("tdqds_1_v"      , &_t_dqds_1_v         );
    _tree->Branch("tdqds_diff_v"   , &_t_dqds_diff_v      );
    _tree->Branch("tdqds_ratio_v"  , &_t_dqds_ratio_v     );
    _tree->Branch("tdqds_diff_01"  , &_t_dqds_diff_01       );
    _tree->Branch("tdqds_ratio_01" , &_t_dqds_ratio_01      );

    _tree->Branch("rdqds_0_v"      , &_r_dqds_0_v         );
    _tree->Branch("rdqds_1_v"      , &_r_dqds_1_v         );
    _tree->Branch("rdqds_diff_v"   , &_r_dqds_diff_v      );
    _tree->Branch("rdqds_ratio_v"  , &_r_dqds_ratio_v     );
    _tree->Branch("rdqds_diff_01"  , &_r_dqds_diff_01       );
    _tree->Branch("rdqds_ratio_01" , &_r_dqds_ratio_01      );

    _tree->Branch("theta"              , &_theta              );  
    _tree->Branch("phi"                , &_phi                );
    _tree->Branch("dqds_0_v_3dc"       , &_dqds_0_v_3dc           );//to check abs dqds
    _tree->Branch("dqds_1_v_3dc"       , &_dqds_1_v_3dc           );//to check abs dqds
    _tree->Branch("dqds_diff_01_3dc"   , &_dqds_diff_01_3dc       );
    _tree->Branch("dqds_ratio_01_3dc"  , &_dqds_ratio_01_3dc      );
    
    _roid = 0;
    
    // Register 3 particle cluster arrays, 1 per plane
    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }
  
  void dQdsAnalysis::Clear(){
    _dqds_0_v.clear();
    _dqds_0_v.resize(3);
    _dqds_1_v.clear();
    _dqds_1_v.resize(3);
    _dqds_diff_v.clear();
    _dqds_diff_v.resize(3,-9999);
    _dqds_ratio_v.clear();
    _dqds_ratio_v.resize(3,-9999);


    _r_dqds_0_v.clear();
    _r_dqds_0_v.resize(3);
    _r_dqds_1_v.clear();
    _r_dqds_1_v.resize(3);
    _r_dqds_diff_v.clear();
    _r_dqds_diff_v.resize(3,-9999);
    _r_dqds_ratio_v.clear();
    _r_dqds_ratio_v.resize(3,-9999);

    _t_dqds_0_v.clear();
    _t_dqds_0_v.resize(3,-9999);
    _t_dqds_1_v.clear();
    _t_dqds_1_v.resize(3,-9999);
    _t_dqds_diff_v.clear();
    _t_dqds_diff_v.resize(3,-9999);
    _t_dqds_ratio_v.clear();
    _t_dqds_ratio_v.resize(3,-9999);
    
    _dqds_0_v_3dc.clear();
    _dqds_0_v_3dc.resize(3);
    _dqds_1_v_3dc.clear();
    _dqds_1_v_3dc.resize(3);
    _dqds_diff_v_3dc.clear();
    _dqds_diff_v_3dc.resize(3,-9999);
    _dqds_ratio_v_3dc.clear();
    _dqds_ratio_v_3dc.resize(3,-9999);
    
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
    
    size_t dqds_size = input_dqds.size();
    
    if (input_dqds.size()>=8){ 
      
      float mean_before = 0;
      float mean_after  = 0;
      for (size_t idx = 0; idx < input_dqds.size()-3; ++idx){
	
	mean_before = (input_dqds[idx]+input_dqds[idx+1]+input_dqds[idx+2])/3;

	mean_after  = (input_dqds[idx+1]+input_dqds[idx+2]+input_dqds[idx+3])/3;

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
    
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();
   // Get the Association Manager
    auto& ass_man = AssManager();
    
    // Get vertex from combined analysis
    std::vector<const data::Vertex3D*> vertex_data_v;
    if(_combined_vertex_analysis_algo_id!=kINVALID_ALGO_ID) {
      const auto& input_vertex_data_v  = AlgoData<data::Vertex3DArray>(_combined_vertex_analysis_algo_id,0).as_vector();
      for(const auto& vtx : input_vertex_data_v)
	vertex_data_v.push_back(&vtx);
    }
    
    // Get 3d info from matchoverlap
    const auto& threeD_data  = AlgoData<data::Info3DArray>(_match_analysis_algo_id,0);
    
    _vtxid = -1;
    
    for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
      Clear();
      
      const auto& vertex3d = vertex_data_v[vertex_id];
      _x = vertex3d->x;
      _y = vertex3d->y;
      _z = vertex3d->z;
      
      const auto& par_data = AlgoData<data::ParticleArray>(_match_overlap_algo_id,0);
      auto par_ass_id_v = ass_man.GetManyAss(*vertex3d,par_data.ID());
      if(par_ass_id_v.size()==0) continue;

      _vtxid += 1;

      // per plane a vector for particle cluster & info3d
      std::vector<std::vector<data::ParticleCluster> > pcluster_vv(3);
      std::vector<std::vector<data::Info3D> >          info3d_vv(3);

      for(auto par_id : par_ass_id_v) {

	const auto& par  = par_data.as_vector().at(par_id);
	
	for(size_t plane=0; plane<3; ++plane) {

	  const auto& pcluster_array = AlgoData<data::ParticleClusterArray>(_angle_analysis_algo_id,plane);

	  auto pcluster_id = ass_man.GetOneAss(par,pcluster_array.ID());

	  if (pcluster_id >= pcluster_array.as_vector().size()) {

	    pcluster_vv.at(plane).push_back(data::ParticleCluster());
	    continue;
	  }
	  const auto& pcluster = pcluster_array.as_vector().at(pcluster_id);
	  pcluster_vv.at(plane).push_back(pcluster);
	}

	const auto info3d_id   = ass_man.GetOneAss(par,threeD_data.ID());

	const auto& this_info3d = threeD_data.as_vector().at(info3d_id);	

	for(size_t plane=0; plane<3; ++plane)
	info3d_vv.at(plane).push_back(this_info3d);
	
      }
      
      for(size_t plane =0; plane <=2; ++plane){
	
	// Input algo data
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	int pid = 0;
	for(size_t par_id = 0; par_id < pcluster_vv.at(plane).size(); par_id++) {
	    
	  auto& par = pcluster_vv.at(plane).at(par_id);
	  auto& this_info3d = info3d_vv.at(plane).at(par_id);
	    
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
	  //Play w/ truncated method
	  float truncated_dqds;
	  truncated_dqds = Calc_truncated_mean(trunk_dqds, 10);
	  */
	  
	  /* 
	  //Play w/ smoothing method
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
	  par._truncated_dqds = remove_dqds;
	  par._dqds_mean = VectorMean(remove_dqds);
	  
	  if(pid == 0){
	    _dqds_0_v[plane] = par._dqds_mean;
	    _dqds_0_v_3dc[plane] = Correct3D(_dqds_0_v[plane], _theta, _phi);
	    //_dqds_0_v_3dc[plane] = Correct3D(truncated_dqds, _theta, _phi);
	    //_r_dqds_0_v[plane] = VectorMean(remove_dqds);
	    //_t_dqds_0_v[plane] = truncated_dqds;
	  }
	  if(pid == 1){
	    _dqds_1_v[plane] = par._dqds_mean;
	    _dqds_1_v_3dc[plane] = Correct3D(_dqds_1_v[plane], _theta, _phi);
	    //_dqds_1_v_3dc[plane] = Correct3D(truncated_dqds, _theta, _phi);
	    //_r_dqds_1_v[plane] = VectorMean(remove_dqds);
	    //_t_dqds_1_v[plane] = truncated_dqds;
	  }

	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  ++pid;
	}

	if(_dqds_0_v[plane]     == 0 ) _dqds_0_v[plane]     = _dqds_1_v[plane]; //stupid way for cases where dqds not calculated
	if(_dqds_1_v[plane]     == 0 ) _dqds_1_v[plane]     = _dqds_0_v[plane]; //stupid way for cases where dqds not calculated
	//if(_t_dqds_0_v[plane]   == 0 ) _t_dqds_0_v[plane]   = _t_dqds_1_v[plane]; //stupid way for cases where dqds not calculated
	//if(_t_dqds_1_v[plane]   == 0 ) _t_dqds_1_v[plane]   = _t_dqds_0_v[plane]; //stupid way for cases where dqds not calculated
	//if(_r_dqds_0_v[plane]   == 0 ) _r_dqds_0_v[plane]   = _r_dqds_1_v[plane]; //stupid way for cases where dqds not calculated
	//if(_r_dqds_1_v[plane]   == 0 ) _r_dqds_1_v[plane]   = _r_dqds_0_v[plane]; //stupid way for cases where dqds not calculated
	if(_dqds_0_v_3dc[plane] == 0 ) _dqds_0_v_3dc[plane] = _dqds_1_v_3dc[plane]; //stupid way for cases where dqds not calculated
	if(_dqds_1_v_3dc[plane] == 0 ) _dqds_1_v_3dc[plane] = _dqds_0_v_3dc[plane]; //stupid way for cases where dqds not calculated


	_dqds_diff_v[plane]     = std::abs(_dqds_0_v[plane]     -  _dqds_1_v[plane]);
	//_t_dqds_diff_v[plane]   = std::abs(_t_dqds_0_v[plane]   -  _t_dqds_1_v[plane]);
	//_r_dqds_diff_v[plane]   = std::abs(_r_dqds_0_v[plane]   -  _r_dqds_1_v[plane]);
	_dqds_diff_v_3dc[plane] = std::abs(_dqds_0_v_3dc[plane] -  _dqds_1_v_3dc[plane]);
	
	_dqds_ratio_v[plane]     = (_dqds_0_v[plane]/_dqds_1_v[plane] <= 1 ? 
				    _dqds_0_v[plane]/_dqds_1_v[plane] : _dqds_1_v[plane]/_dqds_0_v[plane] );
	//_t_dqds_ratio_v[plane]   = (_t_dqds_0_v[plane]/_t_dqds_1_v[plane] <= 1 ? 
	//_t_dqds_0_v[plane]/_t_dqds_1_v[plane] : _t_dqds_1_v[plane]/_t_dqds_0_v[plane] );
	//_r_dqds_ratio_v[plane]   = (_r_dqds_0_v[plane]/_r_dqds_1_v[plane] <= 1 ? 
	//_r_dqds_0_v[plane]/_r_dqds_1_v[plane] : _r_dqds_1_v[plane]/_r_dqds_0_v[plane] );
	_dqds_ratio_v_3dc[plane] = (_dqds_0_v_3dc[plane]/_dqds_1_v_3dc[plane] <= 1 ?
				    _dqds_0_v_3dc[plane]/_dqds_1_v_3dc[plane] : _dqds_1_v_3dc[plane]/_dqds_0_v_3dc[plane] );
	
      }
      
      //if (_dqds_diff_v[0] >= 0 )_dqds_diff_01 = _dqds_diff_v[0];
      //if (_dqds_diff_v[1] > _dqds_diff_01 && _dqds_diff_v[1]>0) _dqds_diff_01 = _dqds_diff_v[1];
      //if (_dqds_ratio_v[0] >= 0 )_dqds_ratio_01 = _dqds_ratio_v[0];
      //if (_dqds_ratio_v[1]< _dqds_ratio_01 && _dqds_ratio_v[1]>0) _dqds_ratio_01 = _dqds_ratio_v[1];

      _dqds_diff_01      = Sort01(_dqds_diff_v)[1];
      _dqds_ratio_01     = Sort01(_dqds_ratio_v)[0];
      //_t_dqds_diff_01    = Sort01(_t_dqds_diff_v)[1];
      //_t_dqds_ratio_01   = Sort01(_t_dqds_ratio_v)[0];
      //_r_dqds_diff_01    = Sort01(_r_dqds_diff_v)[1];
      //_r_dqds_ratio_01   = Sort01(_r_dqds_ratio_v)[0];
      _dqds_diff_01_3dc  = Sort01(_dqds_diff_v_3dc)[1];
      _dqds_ratio_01_3dc = Sort01(_dqds_ratio_v_3dc)[0];
      
      /*
      auto minmax_diff = std::minmax_element(std::begin(_dqds_diff_v), std::end(_dqds_diff_v));
      _dqds_diff_01 = *(minmax_diff.second);
      auto minmax_ratio = std::minmax_element(std::begin(_dqds_ratio_v), std::end(_dqds_ratio_v));
      _dqds_ratio_01 = *(minmax_ratio.first);
      auto minmax_diff_3dc = std::minmax_element(std::begin(_dqds_diff_v_3dc), std::end(_dqds_diff_v_3dc));
      _dqds_diff_01_3dc = *(minmax_diff_3dc.second);
      auto minmax_ratio_3dc = std::minmax_element(std::begin(_dqds_ratio_v_3dc), std::end(_dqds_ratio_v_3dc));
      _dqds_ratio_01_3dc = *(minmax_ratio_3dc.first);
      */

      _tree->Fill();
    }//loop of vertex
    _roid += 1;
  }
  
  float dQdsAnalysis::Correct3D (float dqds_mean, float theta, float phi){
    
    //1st, from phi and theta, get a set of [x,y,z] and x set to 1

    float factor; 
    
    float x = 1.0;
    float y = std::tan(phi);
    float z = std::sqrt((std::pow(x,2)+std::pow(y,2))/std::pow(std::tan(theta), 2));
    
    //Calculate projection factor on xz plane 
    //factor = 1 / std::sqrt( 1+ std::pow(x,2)/std::pow(z,2));

    factor = std::sqrt(std::pow(y,2) + std::pow(z,2)) / std::sqrt(std::pow(x,2) +std::pow(y,2) + std::pow(z,2));

    if (isnan(factor))  factor = 1;
    
    return factor * dqds_mean;

  }


  std::vector<float> dQdsAnalysis::Sort01 ( const std::vector<float> input){
    std::vector<float> res;
    res.clear();

    std::vector<float> data;//size of 2[0,1]
    data.clear();

    for (size_t i = 0; i < 2; ++i){
      if( !isnan(input.at(i) ) )   data.push_back(input.at(i));
    }

    std::sort(data.begin(),data.end(),
	      [](const float& a, const float& b) -> bool
	      {
		return a < b;	      
	      });

    res = data;
    return res;
    
  }
}
#endif
