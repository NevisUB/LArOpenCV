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
#include "LArOpenCV/ImageCluster/AlgoFunction/VectorAnalysis.h"
#include "LArUtil/LArProperties.h"
#include <cassert>

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
    
    _shape_analysis_algo_id = kINVALID_ALGO_ID;
    // Input from AngleAnalysisAlgo
    auto shape_analysis_algo_name = pset.get<std::string>("ShapeAnalysisAlgo","");
    if (!shape_analysis_algo_name.empty()) {
      _shape_analysis_algo_id = this->ID(shape_analysis_algo_name);
      if (_shape_analysis_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given MatchAnalysis name is INVALID!");
    }

    //
    // Configure AtomicAnalysis
    //
    _AtomicAnalysis.Configure(pset.get<Config_t>("AtomicAnalysis"));
    //
    
    _dqds_scan_thre   = pset.get<float>("dQdsScanThre");
    _window_size      = pset.get<double>("TruncateWindowSize");
    _window_frac      = pset.get<double>("CutSize");
    _window_size_thre = pset.get<int>("TruncateWindowSizeThre");
    _head_frac        = pset.get<double>("HeadFrac");
    _tail_frac        = pset.get<double>("TailFrac");
        
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
    _tree->Branch("dqds_diff_01"   , &_dqds_diff_01       ,"dqds_diff_01/F");
    _tree->Branch("dqds_ratio_01"  , &_dqds_ratio_01      ,"dqds_ratio_01/F");

    _tree->Branch("rdqds_0_v"      , &_r_dqds_0_v         );
    _tree->Branch("rdqds_1_v"      , &_r_dqds_1_v         );
    _tree->Branch("rdqds_diff_v"   , &_r_dqds_diff_v      );
    _tree->Branch("rdqds_ratio_v"  , &_r_dqds_ratio_v     );
    _tree->Branch("rdqds_diff_01"  , &_r_dqds_diff_01     ,"rdqds_diff_01/F");
    _tree->Branch("rdqds_ratio_01" , &_r_dqds_ratio_01    ,"rdqds_ratio_01/F");

    _tree->Branch("sdqds_0_v"      , &_s_dqds_0_v         );
    _tree->Branch("sdqds_1_v"      , &_s_dqds_1_v         );
    _tree->Branch("sdqds_diff_v"   , &_s_dqds_diff_v      );
    _tree->Branch("sdqds_ratio_v"  , &_s_dqds_ratio_v     );
    _tree->Branch("sdqds_diff_01"  , &_s_dqds_diff_01     ,"sdqds_diff_01/F");
    _tree->Branch("sdqds_ratio_01" , &_s_dqds_ratio_01    ,"sdqds_ratio_01/F");

    _tree->Branch("tdqds_0_v"      , &_t_dqds_0_v         );
    _tree->Branch("tdqds_1_v"      , &_t_dqds_1_v         );
    _tree->Branch("tdqds_diff_v"   , &_t_dqds_diff_v      );
    _tree->Branch("tdqds_ratio_v"  , &_t_dqds_ratio_v     );
    _tree->Branch("tdqds_diff_01"  , &_t_dqds_diff_01     ,"tdqds_diff_01/F");
    _tree->Branch("tdqds_ratio_01" , &_t_dqds_ratio_01    ,"tdqds_ratio_01/F");
    
    _tree->Branch("theta_0"            , &_theta_0         ,"theta_0/F");  
    _tree->Branch("phi_0"              , &_phi_0           ,"phi_0/F");
    _tree->Branch("theta_1"            , &_theta_1         ,"theta_1/F");  
    _tree->Branch("phi_1"              , &_phi_1           ,"phi_1/F");
    _tree->Branch("dqdx_0_v_3dc"       , &_dqdx_0_v_3dc      );
    _tree->Branch("dqdx_1_v_3dc"       , &_dqdx_1_v_3dc      );
    _tree->Branch("dqdx_diff_01_3dc"   , &_dqdx_diff_01_3dc  ,"dqdx_diff_01_3dc/F");
    _tree->Branch("dqdx_ratio_01_3dc"  , &_dqdx_ratio_01_3dc ,"dqdx_ratio_01_3dc/F");
    
    _tree->Branch("dqdx_0_end_v_3dc"       , &_dqdx_0_end_v_3dc      );
    _tree->Branch("dqdx_1_end_v_3dc"       , &_dqdx_1_end_v_3dc      );
    _tree->Branch("dqdx_diff_end_v_3dc"   , &_dqdx_diff_end_v_3dc  );
    _tree->Branch("dqdx_ratio_end_v_3dc"  , &_dqdx_ratio_end_v_3dc );

    _tree->Branch("trackp_totq"         , &_trackp_totq         ,"trackp_totq/F");
    _tree->Branch("showerp_totq"        , &_showerp_totq        ,"showerp_totq/F");
    _tree->Branch("trackp_cosz"         , &_trackp_cosz         ,"trackp_cosz/F");
    _tree->Branch("showerp_cosz"        , &_showerp_cosz        ,"showerp_cosz/F");

    _tree->Branch("trackp_dqds_v"           , &_trackp_dqds_v       );
    _tree->Branch("showerp_dqds_v"          , &_showerp_dqds_v      );
    _tree->Branch("trackp_dqdx_3dc_v"       , &_trackp_dqdx_3dc_v       );
    _tree->Branch("showerp_dqdx_3dc_v"      , &_showerp_dqdx_3dc_v      );
    
    _tree->Branch("long_trackp_dqds_v"      , &_long_trackp_dqds_v  );
    _tree->Branch("short_trackp_dqds_v"     , &_short_trackp_dqds_v );
    _tree->Branch("long_trackp_dqdx_3dc_v"  , &_long_trackp_dqdx_3dc_v  );
    _tree->Branch("short_trackp_dqdx_3dc_v" , &_short_trackp_dqdx_3dc_v );
    _roid = 0;

    _tree->Branch("dedx_p0"                 , &_dedx_p0);
    _tree->Branch("dedx_p1"                 , &_dedx_p1);
    _tree->Branch("dedx_diff"               , &_dedx_diff);
    _tree->Branch("dedx_trackp"             , &_trackp_dedx);
    _tree->Branch("dedx_showerp"            , &_showerp_dedx);
    _tree->Branch("dedx_long_trackp"        , &_long_trackp_dedx);
    _tree->Branch("dedx_short_trackp"       , &_short_trackp_dedx);

    _tree->Branch("par1_0_qden_scan_v" , &_par1_0_qden_scan_v);
    _tree->Branch("par2_0_qden_scan_v" , &_par2_0_qden_scan_v);
    _tree->Branch("par1_1_qden_scan_v" , &_par1_1_qden_scan_v);
    _tree->Branch("par2_1_qden_scan_v" , &_par2_1_qden_scan_v);
    _tree->Branch("par1_2_qden_scan_v" , &_par1_2_qden_scan_v);
    _tree->Branch("par2_2_qden_scan_v" , &_par2_2_qden_scan_v);
    _tree->Branch("chi2_par1_0"        , &_chi2_par1_0);
    _tree->Branch("chi2_par1_1"        , &_chi2_par1_1);
    _tree->Branch("chi2_par1_2"        , &_chi2_par1_2);
    _tree->Branch("chi2_par2_0"        , &_chi2_par2_0);
    _tree->Branch("chi2_par2_1"        , &_chi2_par2_1);
    _tree->Branch("chi2_par2_2"        , &_chi2_par2_2);
    _tree->Branch("npx_in_vtx_circ_v"  , &_npx_in_vtx_circ_v);
    _tree->Branch("npx_on_vtx_circ_v"  , &_npx_on_vtx_circ_v);

    _nplanes = 3;
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }

  void dQdsAnalysis::ClearEvent(){
    _vtxid = kINVALID_INT;
    ClearVertex();
  }
  
  void dQdsAnalysis::ClearVertex(){
    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

    _dqds_diff_01 = kINVALID_FLOAT;
    _dqds_ratio_01 = kINVALID_FLOAT;

    _r_dqds_diff_01 = kINVALID_FLOAT;
    _r_dqds_ratio_01 = kINVALID_FLOAT;

    _s_dqds_diff_01 = kINVALID_FLOAT;
    _s_dqds_ratio_01 = kINVALID_FLOAT;
    
    _t_dqds_diff_01 = kINVALID_FLOAT;
    _t_dqds_ratio_01 = kINVALID_FLOAT;
    
    _dqds_0_v.clear();
    _dqds_0_v.resize(3);
    _dqds_1_v.clear();
    _dqds_1_v.resize(3);
    _dqds_diff_v.clear();
    _dqds_diff_v.resize(3,-kINVALID_FLOAT);
    _dqds_ratio_v.clear();
    _dqds_ratio_v.resize(3,-kINVALID_FLOAT);

    _r_dqds_0_v.clear();
    _r_dqds_0_v.resize(3);
    _r_dqds_1_v.clear();
    _r_dqds_1_v.resize(3);
    _r_dqds_diff_v.clear();
    _r_dqds_diff_v.resize(3,-kINVALID_FLOAT);
    _r_dqds_ratio_v.clear();
    _r_dqds_ratio_v.resize(3,-kINVALID_FLOAT);

    _s_dqds_0_v.clear();
    _s_dqds_0_v.resize(3);
    _s_dqds_1_v.clear();
    _s_dqds_1_v.resize(3);
    _s_dqds_diff_v.clear();
    _s_dqds_diff_v.resize(3,-kINVALID_FLOAT);
    _s_dqds_ratio_v.clear();
    _s_dqds_ratio_v.resize(3,-kINVALID_FLOAT);

    _t_dqds_0_v.clear();
    _t_dqds_0_v.resize(3,-kINVALID_FLOAT);
    _t_dqds_1_v.clear();
    _t_dqds_1_v.resize(3,-kINVALID_FLOAT);
    _t_dqds_diff_v.clear();
    _t_dqds_diff_v.resize(3,-kINVALID_FLOAT);
    _t_dqds_ratio_v.clear();
    _t_dqds_ratio_v.resize(3,-kINVALID_FLOAT);
    
    _dqdx_0_v_3dc.clear();
    _dqdx_0_v_3dc.resize(3,-kINVALID_FLOAT);
    _dqdx_1_v_3dc.clear();
    _dqdx_1_v_3dc.resize(3,-kINVALID_FLOAT);
    _dqdx_diff_v_3dc.clear();
    _dqdx_diff_v_3dc.resize(3,-kINVALID_FLOAT);
    _dqdx_ratio_v_3dc.clear();
    _dqdx_ratio_v_3dc.resize(3,-kINVALID_FLOAT);

    _dqdx_0_end_v_3dc.clear();
    _dqdx_0_end_v_3dc.resize(3,-kINVALID_FLOAT);
    _dqdx_1_end_v_3dc.clear();
    _dqdx_1_end_v_3dc.resize(3,-kINVALID_FLOAT);
    _dqdx_diff_end_v_3dc.clear();
    _dqdx_diff_end_v_3dc.resize(3,-kINVALID_FLOAT);
    _dqdx_ratio_end_v_3dc.clear();
    _dqdx_ratio_end_v_3dc.resize(3,-kINVALID_FLOAT);

    _trackp_dqds_v.clear();
    _trackp_dqds_v.resize(3,-kINVALID_FLOAT);
    _showerp_dqds_v.clear();
    _showerp_dqds_v.resize(3,-kINVALID_FLOAT);
    
    _trackp_dqdx_3dc_v.clear();
    _trackp_dqdx_3dc_v.resize(3,-kINVALID_FLOAT);
    _showerp_dqdx_3dc_v.clear();
    _showerp_dqdx_3dc_v.resize(3,-kINVALID_FLOAT);
    
    _long_trackp_dqdx_3dc_v.clear();
    _long_trackp_dqdx_3dc_v.resize(3,-kINVALID_FLOAT);
    _short_trackp_dqdx_3dc_v.clear();
    _short_trackp_dqdx_3dc_v.resize(3,-kINVALID_FLOAT);
    
    _long_trackp_dqds_v.clear();
    _long_trackp_dqds_v.resize(3,-kINVALID_FLOAT);
    _short_trackp_dqds_v.clear();
    _short_trackp_dqds_v.resize(3,-kINVALID_FLOAT);
    
    _theta_0 = kINVALID_FLOAT;
    _phi_0 = kINVALID_FLOAT;
    _theta_1 = kINVALID_FLOAT;
    _phi_1 = kINVALID_FLOAT;
    
    _dqdx_diff_01_3dc = kINVALID_FLOAT;
    _dqdx_ratio_01_3dc = kINVALID_FLOAT;
    
    _trackp_totq = kINVALID_FLOAT;
    _showerp_totq = kINVALID_FLOAT;
    _trackp_cosz = kINVALID_FLOAT;
    _showerp_cosz = kINVALID_FLOAT;

    _par1_0_qden_scan_v.clear();
    _par2_0_qden_scan_v.clear();
    _par1_1_qden_scan_v.clear();
    _par2_1_qden_scan_v.clear();
    _par1_2_qden_scan_v.clear();
    _par2_2_qden_scan_v.clear();

    _chi2_par1_0 = kINVALID_FLOAT;
    _chi2_par1_1 = kINVALID_FLOAT;
    _chi2_par1_2 = kINVALID_FLOAT;
    _chi2_par2_0 = kINVALID_FLOAT;
    _chi2_par2_1 = kINVALID_FLOAT;
    _chi2_par2_2 = kINVALID_FLOAT;

    _npx_in_vtx_circ_v.clear();
    _npx_in_vtx_circ_v.resize(3,-kINVALID_FLOAT);

    _npx_on_vtx_circ_v.clear();
    _npx_on_vtx_circ_v.resize(3,-kINVALID_FLOAT);
    
  }

  bool dQdsAnalysis::_PostProcess_() const
  { return true; }

  void dQdsAnalysis::_Process_() {
    LAROCV_INFO() << "start" << std::endl;
    ClearEvent();
    
    if(NextEvent()) _roid =0;
    
    auto img_v  = ImageArray();
    //_image_array = img_v;
    
    
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
    const auto& twoD_data    = AlgoData<data::Info2DArray>(_shape_analysis_algo_id,0);
    
    _vtxid = -1;
    
    for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
      ClearVertex();
      
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
      std::vector<std::vector<data::Info2D> >          info2d_vv(3);
      
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
	const auto info2d_id   = ass_man.GetOneAss(par,twoD_data.ID());
	
	const auto& this_info3d = threeD_data.as_vector().at(info3d_id);
	const auto& this_info2d = twoD_data.as_vector().at(info2d_id);
	
	for(size_t plane=0; plane<3; ++plane){
	  info3d_vv.at(plane).push_back(this_info3d);
	  info2d_vv.at(plane).push_back(this_info2d);
	}
      }
      
      for(size_t plane =0; plane <=2; ++plane){
	
	// Input algo data
	
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane);
	
	int pid = 0;
	float  length0 ; 
	float  length1 ;
	for(size_t par_id = 0; par_id < pcluster_vv.at(plane).size(); par_id++) {
	    
	  auto& par = pcluster_vv.at(plane).at(par_id);
	  auto& this_info3d = info3d_vv.at(plane).at(par_id);
	  auto& this_info2d = info2d_vv.at(plane).at(par_id);
	    
	  //auto end_pt = this_info3d.overall_pca_end_pt;
	  auto theta  = this_info3d.trunk_pca_theta;
	  auto phi    = this_info3d.trunk_pca_phi;
	  auto cosz   = std::cos(theta);
	  
	  auto trunk_space_pts_v = this_info3d.trunk_space_pts_v;
	  auto threeD_pca_start_pt = this_info3d.trunk_pca_start_pt;
	  auto threeD_pca_end_pt   = this_info3d.trunk_pca_end_pt;

	  float converted_dedx = -1 * kINVALID_FLOAT;
	  if( converted_dedx == -1 * kINVALID_FLOAT ){
	    std::vector<std::array<float,3> > projected_v;
	    projected_v.reserve(trunk_space_pts_v.size());
	    
	    for (auto this_3d_pt : trunk_space_pts_v ){
	      auto pt = AsVector(this_3d_pt.pt.x,this_3d_pt.pt.y,this_3d_pt.pt.z);
	      projected_v.emplace_back(ClosestPoint(threeD_pca_start_pt,
						    threeD_pca_end_pt,
						    pt));
	    }
	    
	    float max_dist = -1.0*kINVALID_FLOAT;
	    float min_dist = kINVALID_FLOAT;
	    
	    auto vertex_pt = AsVector(vertex3d->x, 
				      vertex3d->y, 
				      vertex3d->z);
	    
	    for (auto this_proj_pt : projected_v){
	      auto distance = Distance(this_proj_pt,vertex_pt);
	      if (distance > max_dist) max_dist = distance;
	      if (distance < min_dist) min_dist = distance;
	    }
	    
	    float remove_dedx_mean = -kINVALID_FLOAT;
	    
	    if(max_dist > min_dist) {
	      
	      size_t num_bins = (max_dist - min_dist) / 0.3 + 1; //resolution
	      std::vector<float> dedx(num_bins, 0.);
	      
	      for (auto this_3d_pt : trunk_space_pts_v ){
		auto pt = AsVector(this_3d_pt.pt.x,this_3d_pt.pt.y,this_3d_pt.pt.z);
		auto this_proj_pt = ClosestPoint(threeD_pca_start_pt,
						 threeD_pca_end_pt,
						 pt);
		auto distance = Distance(this_proj_pt,vertex_pt);
		size_t bin = (distance - min_dist) / 0.3 ; //resolution
		
		dedx.at(bin) += this_3d_pt.q;
	      }
	      
	      std::vector<float> remove_dedx;
	      remove_dedx.clear();
	      remove_dedx = CutHeads(dedx, _head_frac, _tail_frac);
	    
	      remove_dedx_mean =  VectorMean(remove_dedx);
	    }
	    
	    par._dedx = remove_dedx_mean;
	    
	    converted_dedx = larutil::LArProperties::GetME()->ModBoxCorrection(remove_dedx_mean*200*6);

	  }
	  //dedx above

	  cv::Mat masked_ctor;

	  masked_ctor = MaskImage(img_v.at(plane),par._ctor,0,false); 	
	  
	  data::AtomicContour raw_atom;
	  
	  raw_atom = FindNonZero(masked_ctor);
	  
	  par._atom = raw_atom;

	  float tot_q = 0;
	  
	  for(auto pt : raw_atom) {
	    float q = (float)(masked_ctor.at<unsigned char>((size_t)(pt.y),(size_t)(pt.x)));
	    tot_q +=q;
	  }
	  
	  if (this_info2d.ptype == data::ParticleType_t::kTrack){
	    _trackp_totq = tot_q;
	    _trackp_cosz = cosz;
	    _trackp_dedx = converted_dedx;
	  }
	  if (this_info2d.ptype == data::ParticleType_t::kShower){
	    _showerp_totq = tot_q;
	    _showerp_cosz = cosz;
	    _showerp_dedx = converted_dedx;
	  }

	  geo2d::Circle<float> circle;
	  
	  circle = par._circle;

	  masked_ctor = MaskImage(masked_ctor,circle,0,false); 	
	  //masked_ctor = Threshold(masked_ctor, 10, 255);
	  
	  if ( FindNonZero(masked_ctor).size() <2 ) continue;
	  data::AtomicContour thisatom;
	  thisatom = FindNonZero(masked_ctor);
	  
	  auto this_pca = CalcPCA(thisatom);
	  auto start_point = circle.center;
	  geo2d::Vector<float> angle_scan_end_point;
	  
	  angle_scan_end_point = par._angle_scan_end_point;
	  
	  if (int(par._angle_scan_end_point.x == -kINVALID_FLOAT)){
	    FindEdge(thisatom, start_point, angle_scan_end_point);
	  }
	  geo2d::Vector<float> atom_end_point;
	  FindEdge(raw_atom, start_point, atom_end_point);
	  float length = std::sqrt(std::pow(start_point.x - atom_end_point.x , 2) + 
				   std::pow(start_point.y - atom_end_point.y , 2) );
	  par._atom_end_point = atom_end_point;

	  std::vector<float> par_dqds;
	  par_dqds.clear();
	  par_dqds = _AtomicAnalysis.AtomdQdX(masked_ctor, thisatom, this_pca, start_point, angle_scan_end_point);

	  // Do bragg search
	  cv::Mat this_raw_ctor;
	  cv::Mat vtxCirc_ctor;
	  cv::Mat vtxRing_ctor;
	  this_raw_ctor = MaskImage(img_v.at(plane),par._ctor,0,false);

	  geo2d::Circle<float> vtxCirc;
	  geo2d::Circle<float> vtxCircSmall;
	  int npxInVtxCirc;
	  int npxOnVtxCirc;
	  vtxCirc.center      = start_point;
	  vtxCirc.radius      = 6;
	  vtxCircSmall.center = start_point;
	  vtxCircSmall.radius = 5;

	  vtxCirc_ctor   = MaskImage(this_raw_ctor,vtxCirc,0,false);
	  vtxRing_ctor   = MaskImage(vtxCirc_ctor,vtxCircSmall,0,true);
	  npxInVtxCirc   = CountNonZero(vtxCirc_ctor);
	  npxOnVtxCirc   = CountNonZero(vtxRing_ctor);
	  
	  if (_npx_in_vtx_circ_v.at(plane) == -kINVALID_FLOAT) {
	    _npx_in_vtx_circ_v.at(plane)  = npxInVtxCirc;
	  } else {
	    _npx_in_vtx_circ_v.at(plane) += npxInVtxCirc;
	  }

	  if (_npx_on_vtx_circ_v.at(plane) == -kINVALID_FLOAT) {
	    _npx_on_vtx_circ_v.at(plane)  = npxOnVtxCirc;
	  } else {
	    _npx_on_vtx_circ_v.at(plane) += npxOnVtxCirc;
	  }


	  auto this_track_atom = FindNonZero(this_raw_ctor);
	  auto this_track_pca  = CalcPCA(this_track_atom);
	  float this_slope     = std::tan(angle(this_track_pca)*3.14159/180);
	  float this_offset    = this_track_pca.y(0.0);
	  float stndA          = 1.0;
	  float stndB          = -1.0/this_slope;
	  float stndC          = this_offset/this_slope;
	  
	  float this_chi2  = 0;
	  float atm_points = 0;
	  for (auto pt : this_track_atom) {
	    atm_points++;
	  }

	  for (auto pt : this_track_atom) {
	    //auto pt_dist = Distance(this_track_pca, pt);
	    float pt_x = (float) pt.x;
	    float pt_y = (float) pt.y;

	    float distToLine = std::abs(pt_x + stndB*pt_y + stndC) / std::sqrt(1.0+stndB*stndB);

	    this_chi2+=distToLine/atm_points;
	  }

	  int circleRad   = 5;
	  int circleStep  = 2;

	  std::vector<float>trackDensities;

	  int skipCounter = 0;
	  for (auto pt : this_track_atom) {

	    if (skipCounter%circleStep != 0){
	      skipCounter++;
	      continue;
	    }
	    skipCounter++;

	    geo2d::Circle<float> scan_circle;
	    scan_circle.center = pt;
	    scan_circle.radius = circleRad;

	    cv::Mat inCirc_ctor;
	    inCirc_ctor = MaskImage(this_raw_ctor,scan_circle,0,false);

	    size_t qInCirc;
	    float  npxInCirc;
	    float  density;

	    qInCirc   = SumNonZero(inCirc_ctor);
	    npxInCirc = CountNonZero(inCirc_ctor);
	    density   = qInCirc / npxInCirc;

	    if (std::isnan(density)) {
	      trackDensities.push_back(-1.0);
	    } else {
	      trackDensities.push_back(density);
	    }
	  }// Finish Do Bragg Search
	  	  
	  std::vector<float> trunk_dqds;
	  trunk_dqds.clear();
	  if (par_dqds.size() > 30){
	    for(size_t idx = 0; idx <30 ; ++idx ) trunk_dqds.push_back(par_dqds.at(idx));
	  }
	  else {trunk_dqds = par_dqds;};
	  
	  std::vector<float> remove_dqds;
	  remove_dqds.clear();
	  remove_dqds = CutHeads(trunk_dqds, _head_frac, _tail_frac);
	  
	  // dQds Spectrum smooth method
	  std::vector<float> smooth_dqds;
	  smooth_dqds.clear();
	  if(trunk_dqds.size())
	    smooth_dqds = Calc_smooth_mean(trunk_dqds, _window_size,_window_size_thre,_window_frac);

	  std::vector<float> peak_finder_dqds;
	  peak_finder_dqds.clear();
	  peak_finder_dqds = PeakFinder(trunk_dqds,0.1);
	  
	  par._vertex_dqds = par_dqds;
	  par._truncated_dqds = remove_dqds;

	  float trunk_mean_dqds     = VectorMean(trunk_dqds);
	  float remove_mean_dqds    = VectorMean(remove_dqds);
	  float smooth_mean_dqds    = VectorMean(smooth_dqds);
	  float truncated_mean_dqds = Calc_truncated_mean(trunk_dqds, 10);
 	  float mean_dqds_3dc       = Correct3D(remove_mean_dqds, theta, phi);
	  
	  par._dqds_mean = remove_mean_dqds;

	  // For end dqds
	  // Get the image again
	  cv::Mat raw_ctor;
	  raw_ctor = MaskImage(img_v.at(plane),par._ctor,0,false); 	
	  // Draw a circle at the end point to mask out bragg peak and michel etc. 
	  geo2d::Circle<float> end_circle;
	  
	  end_circle.center = atom_end_point;
	  end_circle.radius = 10;

	  masked_ctor = MaskImage(raw_ctor,end_circle,0,true); 	
	  
	  geo2d::Vector<float> in_start;
	  geo2d::Vector<float> in_end;
	  
	  FindEdges(raw_atom, in_start, in_end);
	  
	  float end_radius = 40;
	  
	  if (std::sqrt(std::pow(in_start.x - in_end.x,2) + std::pow(in_start.y - in_end.y,2)) < 40)
	    {
	      end_radius = std::sqrt(std::pow(in_start.x - in_end.x,2) + std::pow(in_start.y - in_end.y,2));
	    }
	  
	  end_circle.radius = end_radius;
	  masked_ctor = MaskImage(masked_ctor,end_circle,0,false);
	  auto end_pca = CalcPCA(thisatom);
	  
	  data::AtomicContour endatom;
	  endatom = FindNonZero(masked_ctor);
	  
	  std::vector<float> end_par_dqds;

	  geo2d::Vector<float> end_A;
	  geo2d::Vector<float> end_B;
	  
	  float a = kINVALID_FLOAT,  b = 0;
	  
	  for (auto const pt : endatom){
	    geo2d::Vector<float> pt_f;
	    
	    pt_f.x = (float) pt.x;
	    pt_f.y = (float) pt.y;
	    
	    if (geo2d::dist(pt_f, atom_end_point) < a) end_A = pt;
	    if (geo2d::dist(pt_f, atom_end_point) > b) end_B = pt;
	  }
	  
	  end_par_dqds.clear();
	  end_par_dqds = _AtomicAnalysis.AtomdQdX(masked_ctor, endatom, end_pca, end_A, end_B);
	  
	  std::vector<float> remove_end_dqds;
	  remove_end_dqds.clear();
	  remove_end_dqds = CutHeads(end_par_dqds, _head_frac, _tail_frac);
	  auto mean_end_dqds = VectorMean(remove_end_dqds);
	  auto end_dqds_3dc = Correct3D(mean_end_dqds, theta, phi);	    
	  
	  // Filling
	  if(pid == 0){
	    _dqds_0_v.at(plane)     = trunk_mean_dqds;
	    _r_dqds_0_v.at(plane)   = remove_mean_dqds;
	    _s_dqds_0_v.at(plane)   = smooth_mean_dqds;
	    _t_dqds_0_v.at(plane)   = truncated_mean_dqds;
	    _dqdx_0_v_3dc.at(plane) = mean_dqds_3dc;
	    _dqdx_0_end_v_3dc.at(plane) = end_dqds_3dc;
	    _theta_0 = theta;
	    _phi_0   = phi;
	    length0  = length;
	    _dedx_p0 = converted_dedx;
	    
	    if(plane == 0){
	      _par1_0_qden_scan_v = trackDensities;
	      _chi2_par1_0 = this_chi2;
	    }
	    if(plane == 1){
	      _par1_1_qden_scan_v = trackDensities;
	      _chi2_par1_1 = this_chi2;
	    }
	    if(plane == 2){
	      _par1_2_qden_scan_v = trackDensities;
	      _chi2_par1_2 = this_chi2;
	    }
	  }
	  
	  if(pid == 1){
	    _dqds_1_v.at(plane)     = trunk_mean_dqds;
	    _r_dqds_1_v.at(plane)   = remove_mean_dqds;
	    _s_dqds_1_v.at(plane)   = smooth_mean_dqds;
	    _t_dqds_1_v.at(plane)   = truncated_mean_dqds;
	    _dqdx_1_v_3dc.at(plane) = mean_dqds_3dc;
	    _dqdx_1_end_v_3dc.at(plane) = end_dqds_3dc;
	    _theta_1 = theta;
	    _phi_1   = phi;
	    length1  = length;
	    _dedx_p1 = converted_dedx;
	    
	    if(plane == 0){
	      _par2_0_qden_scan_v = trackDensities;
	      _chi2_par2_0 = this_chi2;
	    }
	    if(plane == 1){
	      _par2_1_qden_scan_v = trackDensities;
	      _chi2_par2_1 = this_chi2;
	    }
	    if(plane == 2){
	      _par2_2_qden_scan_v = trackDensities;
	      _chi2_par2_2 = this_chi2;
	    }
	  }
	  
	  if (this_info2d.ptype == data::ParticleType_t::kTrack){
	    _trackp_dqds_v.at(plane)  = remove_mean_dqds;//Correct3D(par._dqds_mean, theta, phi);
	    _trackp_dqdx_3dc_v.at(plane)  = mean_dqds_3dc;//Correct3D(par._dqds_mean, theta, phi);
	  }
	  if (this_info2d.ptype == data::ParticleType_t::kShower){
	    _showerp_dqds_v.at(plane) = remove_mean_dqds;//Correct3D(par._dqds_mean, theta, phi);
	    _showerp_dqdx_3dc_v.at(plane) = mean_dqds_3dc;//Correct3D(par._dqds_mean, theta, phi);
	  }

	  this_par_data.push_back(par);
	  AssociateMany(*vertex3d,this_par_data.as_vector().back());
	  ++pid;
	}

	if(pid == 2){
	  if (length0 >= length1) {
	    _long_trackp_dqds_v.at(plane)  = _dqds_0_v.at(plane);
	    _short_trackp_dqds_v.at(plane) = _dqds_1_v.at(plane);
	    _long_trackp_dqdx_3dc_v.at(plane)  = _dqdx_0_v_3dc.at(plane);
	    _short_trackp_dqdx_3dc_v.at(plane) = _dqdx_1_v_3dc.at(plane);
	    _long_trackp_dedx  = _dedx_p0;
	    _short_trackp_dedx = _dedx_p1;
	    
	  }else{
	    _long_trackp_dqds_v.at(plane)  = _dqds_1_v.at(plane);
	    _short_trackp_dqds_v.at(plane) = _dqds_0_v.at(plane);
	    _long_trackp_dqdx_3dc_v.at(plane)  = _dqdx_1_v_3dc.at(plane);
	    _short_trackp_dqdx_3dc_v.at(plane) = _dqdx_0_v_3dc.at(plane);
	    _long_trackp_dedx  = _dedx_p1;
	    _short_trackp_dedx = _dedx_p0;
	  } 
	  _dedx_diff = std::abs(_dedx_p1-_dedx_p0);
	}
	
	_dqds_diff_v.at(plane)     = std::abs(_dqds_0_v.at(plane)     -  _dqds_1_v.at(plane));
	_r_dqds_diff_v.at(plane)   = std::abs(_r_dqds_0_v.at(plane)   -  _r_dqds_1_v.at(plane));
	_s_dqds_diff_v.at(plane)   = std::abs(_s_dqds_0_v.at(plane)   -  _s_dqds_1_v.at(plane));
	_t_dqds_diff_v.at(plane)   = std::abs(_t_dqds_0_v.at(plane)   -  _t_dqds_1_v.at(plane));
	_dqdx_diff_v_3dc.at(plane) = std::abs(_dqdx_0_v_3dc.at(plane) -  _dqdx_1_v_3dc.at(plane));
	
	_dqds_ratio_v.at(plane)     = (_dqds_0_v.at(plane)/_dqds_1_v.at(plane) <= 1 ? 
				       _dqds_0_v.at(plane)/_dqds_1_v.at(plane) : _dqds_1_v.at(plane)/_dqds_0_v.at(plane) );
	_r_dqds_ratio_v.at(plane)   = (_r_dqds_0_v.at(plane)/_r_dqds_1_v.at(plane) <= 1 ? 
				       _r_dqds_0_v.at(plane)/_r_dqds_1_v.at(plane) : _r_dqds_1_v.at(plane)/_r_dqds_0_v.at(plane) );
	_s_dqds_ratio_v.at(plane)   = (_s_dqds_0_v.at(plane)/_s_dqds_1_v.at(plane) <= 1 ? 
				       _s_dqds_0_v.at(plane)/_s_dqds_1_v.at(plane) : _s_dqds_1_v.at(plane)/_s_dqds_0_v.at(plane) );
	_t_dqds_ratio_v.at(plane)   = (_t_dqds_0_v.at(plane)/_t_dqds_1_v.at(plane) <= 1 ? 
				       _t_dqds_0_v.at(plane)/_t_dqds_1_v.at(plane) : _t_dqds_1_v.at(plane)/_t_dqds_0_v.at(plane) );
	_dqdx_ratio_v_3dc.at(plane) = (_dqdx_0_v_3dc.at(plane)/_dqdx_1_v_3dc.at(plane) <= 1 ?
				       _dqdx_0_v_3dc.at(plane)/_dqdx_1_v_3dc.at(plane) : _dqdx_1_v_3dc.at(plane)/_dqdx_0_v_3dc.at(plane) );
	
      }
     
      AssignDiff(_dqds_diff_v, _dqds_diff_01);
      AssignRatio(_dqds_ratio_v, _dqds_ratio_01);
      
      AssignDiff(_r_dqds_diff_v, _r_dqds_diff_01);
      AssignRatio(_r_dqds_ratio_v, _r_dqds_ratio_01);

      AssignDiff(_s_dqds_diff_v, _s_dqds_diff_01);
      AssignRatio(_s_dqds_ratio_v, _s_dqds_ratio_01);

      AssignDiff(_t_dqds_diff_v, _t_dqds_diff_01);
      AssignRatio(_t_dqds_ratio_v, _t_dqds_ratio_01);
      
      AssignDiff(_dqdx_diff_v_3dc, _dqdx_diff_01_3dc);
      AssignRatio(_dqdx_ratio_v_3dc, _dqdx_ratio_01_3dc);

      _tree->Fill();
    }// Finish Vertex Loop
    _roid += 1;
  }
  
  // Scan dQds Spectrum by 3 points with step = 1
  std::vector<float> dQdsAnalysis::PeakFinder(std::vector<float> input_dqds, double frac){
    
    size_t dqds_size = input_dqds.size();
    
    if (input_dqds.size()>=8){ 
      
      float mean_before = 0;
      float mean_after  = 0;
      for (size_t idx = 0; idx < input_dqds.size()-3; ++idx){
	
	mean_before = (input_dqds.at(idx)+input_dqds[idx+1]+input_dqds[idx+2])/3;

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
      res.at(idx) = input_dqds.at(idx);
    }
    return res;
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

    if (std::isnan(factor))  factor = 1;
    
    return factor * dqds_mean;

  }

  //Sort output from small to big
  std::vector<float> dQdsAnalysis::Sort01 ( const std::vector<float> input){
    
    std::vector<float> data;
    data.clear();
    
    for (size_t i = 0; i < 2; ++i){
      if(!std::isnan(input.at(i))) data.push_back(input.at(i));
    }

    std::sort(data.begin(),data.end(),
	      [](const float& a, const float& b) -> bool
	      {
		return a < b;	      
	      });
    return data;
    
  }

  void dQdsAnalysis::AssignRatio (const std::vector<float> input,
				  float& output){
    if (!input.empty()) {
      auto res = Sort01(input);
      if (!res.empty())
	output = res[0];
      else
	output = kINVALID_DOUBLE;
    } else
      output = kINVALID_DOUBLE;
  }
  

  void dQdsAnalysis::AssignDiff (const std::vector<float> input,
				 float& output){
    if (!input.empty()) {
      auto res = Sort01(input);
      if (!res.empty())
	output = res[1];
      else
	output = kINVALID_DOUBLE;
    } else
	output = kINVALID_DOUBLE;
  }
}
#endif
