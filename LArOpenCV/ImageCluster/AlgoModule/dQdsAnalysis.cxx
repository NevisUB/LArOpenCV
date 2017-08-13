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
    _drop_location    = pset.get<size_t>("dQdsDropLocation");
    _window_size      = pset.get<double>("TruncateWindowSize");
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

    _tree->Branch("tdqds_0_v"      , &_t_dqds_0_v         );
    _tree->Branch("tdqds_1_v"      , &_t_dqds_1_v         );
    _tree->Branch("tdqds_diff_v"   , &_t_dqds_diff_v      );
    _tree->Branch("tdqds_ratio_v"  , &_t_dqds_ratio_v     );
    _tree->Branch("tdqds_diff_01"  , &_t_dqds_diff_01     ,"tdqds_diff_01/F");
    _tree->Branch("tdqds_ratio_01" , &_t_dqds_ratio_01    ,"tdqds_ratio_01/F");

    _tree->Branch("rdqds_0_v"      , &_r_dqds_0_v         );
    _tree->Branch("rdqds_1_v"      , &_r_dqds_1_v         );
    _tree->Branch("rdqds_diff_v"   , &_r_dqds_diff_v      );
    _tree->Branch("rdqds_ratio_v"  , &_r_dqds_ratio_v     );
    _tree->Branch("rdqds_diff_01"  , &_r_dqds_diff_01     ,"rdqds_diff_01/F");
    _tree->Branch("rdqds_ratio_01" , &_r_dqds_ratio_01    ,"rdqds_ratio_01/F");
    
    _tree->Branch("theta_0"            , &_theta_0         ,"theta_0/F");  
    _tree->Branch("phi_0"              , &_phi_0           ,"phi_0/F");
    _tree->Branch("theta_1"            , &_theta_1         ,"theta_1/F");  
    _tree->Branch("phi_1"              , &_phi_1           ,"phi_1/F");
    _tree->Branch("dqds_0_v_3dc"       , &_dqdx_0_v_3dc      );//to check abs dqds
    _tree->Branch("dqds_1_v_3dc"       , &_dqdx_1_v_3dc      );//to check abs dqds
    _tree->Branch("dqds_diff_01_3dc"   , &_dqdx_diff_01_3dc  ,"dqdx_diff_01_3dc/F");
    _tree->Branch("dqds_ratio_01_3dc"  , &_dqdx_ratio_01_3dc ,"dqdx_ratio_01_3dc/F");
    
    _tree->Branch("dqds_0_end_v_3dc"       , &_dqdx_0_end_v_3dc      );//END dQ/ds
    _tree->Branch("dqds_1_end_v_3dc"       , &_dqdx_1_end_v_3dc      );
    _tree->Branch("dqds_diff_end_v_3dc"   , &_dqdx_diff_end_v_3dc  );
    _tree->Branch("dqds_ratio_end_v_3dc"  , &_dqdx_ratio_end_v_3dc );

    _tree->Branch("trackp_totq"         , &_trackp_totq         ,"trackp_totq/F");
    _tree->Branch("showerp_totq"        , &_showerp_totq        ,"showerp_totq/F");
    _tree->Branch("trackp_cosz"         , &_trackp_cosz         ,"trackp_cosz/F");
    _tree->Branch("showerp_cosz"        , &_showerp_cosz        ,"showerp_cosz/F");

    _tree->Branch("trackp_dqds_v"           , &_trackp_dqds_v       );
    _tree->Branch("showerp_dqds_v"          , &_showerp_dqds_v      );
    _tree->Branch("trackp_dqds_3dc_v"       , &_trackp_dqdx_3dc_v       );
    _tree->Branch("showerp_dqds_3dc_v"      , &_showerp_dqdx_3dc_v      );
    
    _tree->Branch("long_trackp_dqds_v"      , &_long_trackp_dqds_v  );
    _tree->Branch("short_trackp_dqds_v"     , &_short_trackp_dqds_v );
    _tree->Branch("long_trackp_dqdx_3dc_v"  , &_long_trackp_dqdx_3dc_v  );
    _tree->Branch("short_trackp_dqdx_3dc_v" , &_short_trackp_dqdx_3dc_v );
    _roid = 0;

    // may be removed if larcv data product file works
    // _tree->Branch("vertex_v"                  , &_vertex_v);
    // _tree->Branch("particle0_end_point_v"     , &_particle0_end_point);
    // _tree->Branch("particle1_end_point_v"     , &_particle1_end_point);
    //_tree->Branch("particle0_pixels_v"        , &_particle0_pixels_v);
    //_tree->Branch("particle1_pixels_v"        , &_particle1_pixels_v);


    // may be removed if larcv data product file works
    // _tree->Branch("particle0_end_x"           , &_particle0_end_x,"particle0_end_x/D");
    // _tree->Branch("particle0_end_y"           , &_particle0_end_y,"particle0_end_y/D");
    // _tree->Branch("particle0_end_z"           , &_particle0_end_z,"particle0_end_z/D");
    // _tree->Branch("particle1_end_x"           , &_particle1_end_x,"particle1_end_x/D");
    // _tree->Branch("particle1_end_y"           , &_particle1_end_y,"particle1_end_y/D");
    // _tree->Branch("particle1_end_z"           , &_particle1_end_z,"particle1_end_z/D");

    // _tree->Branch("image_particle0_plane0_tmp_x"      , &_image_particle0_plane0_tmp_x);
    // _tree->Branch("image_particle0_plane0_tmp_y"      , &_image_particle0_plane0_tmp_y);
    // _tree->Branch("image_particle0_plane0_tmp_v"      , &_image_particle0_plane0_tmp_v);
    // _tree->Branch("image_particle0_plane1_tmp_x"      , &_image_particle0_plane1_tmp_x);
    // _tree->Branch("image_particle0_plane1_tmp_y"      , &_image_particle0_plane1_tmp_y);
    // _tree->Branch("image_particle0_plane1_tmp_v"      , &_image_particle0_plane1_tmp_v);
    // _tree->Branch("image_particle0_plane2_tmp_x"      , &_image_particle0_plane2_tmp_x);
    // _tree->Branch("image_particle0_plane2_tmp_y"      , &_image_particle0_plane2_tmp_y);
    // _tree->Branch("image_particle0_plane2_tmp_v"      , &_image_particle0_plane2_tmp_v);

    // _tree->Branch("image_particle1_plane0_tmp_x"      , &_image_particle1_plane0_tmp_x);
    // _tree->Branch("image_particle1_plane0_tmp_y"      , &_image_particle1_plane0_tmp_y);
    // _tree->Branch("image_particle1_plane0_tmp_v"      , &_image_particle1_plane0_tmp_v);
    // _tree->Branch("image_particle1_plane1_tmp_x"      , &_image_particle1_plane1_tmp_x);
    // _tree->Branch("image_particle1_plane1_tmp_y"      , &_image_particle1_plane1_tmp_y);
    // _tree->Branch("image_particle1_plane1_tmp_v"      , &_image_particle1_plane1_tmp_v);
    // _tree->Branch("image_particle1_plane2_tmp_x"      , &_image_particle1_plane2_tmp_x);
    // _tree->Branch("image_particle1_plane2_tmp_y"      , &_image_particle1_plane2_tmp_y);
    // _tree->Branch("image_particle1_plane2_tmp_v"      , &_image_particle1_plane2_tmp_v);
    
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
    
    _dqdx_0_v_3dc.clear();
    _dqdx_0_v_3dc.resize(3,-9999);
    _dqdx_1_v_3dc.clear();
    _dqdx_1_v_3dc.resize(3,-9999);
    _dqdx_diff_v_3dc.clear();
    _dqdx_diff_v_3dc.resize(3,-9999);
    _dqdx_ratio_v_3dc.clear();
    _dqdx_ratio_v_3dc.resize(3,-9999);

    _dqdx_0_end_v_3dc.clear();
    _dqdx_0_end_v_3dc.resize(3,-9999);
    _dqdx_1_end_v_3dc.clear();
    _dqdx_1_end_v_3dc.resize(3,-9999);
    _dqdx_diff_end_v_3dc.clear();
    _dqdx_diff_end_v_3dc.resize(3,-9999);
    _dqdx_ratio_end_v_3dc.clear();
    _dqdx_ratio_end_v_3dc.resize(3,-9999);
    

    _trackp_dqds_v.clear();
    _trackp_dqds_v.resize(3,-9999);
    _showerp_dqds_v.clear();
    _showerp_dqds_v.resize(3,-9999);
    
    _trackp_dqdx_3dc_v.clear();
    _trackp_dqdx_3dc_v.resize(3,-9999);
    _showerp_dqdx_3dc_v.clear();
    _showerp_dqdx_3dc_v.resize(3,-9999);
    
    _long_trackp_dqdx_3dc_v.clear();
    _long_trackp_dqdx_3dc_v.resize(3,-9999);
    _short_trackp_dqdx_3dc_v.clear();
    _short_trackp_dqdx_3dc_v.resize(3,-9999);
    
    _long_trackp_dqds_v.clear();
    _long_trackp_dqds_v.resize(3,-9999);
    _short_trackp_dqds_v.clear();
    _short_trackp_dqds_v.resize(3,-9999);
    
    _vertex_v.clear();
    _vertex_v.resize(3);
    
    _particle0_end_point.clear();
    _particle0_end_point.resize(3);
    
    _particle1_end_point.clear();    
    _particle1_end_point.resize(3);
    
    _particle0_pixels_v.clear();
    _particle0_pixels_v.resize(3);
    
    _particle1_pixels_v.clear();
    _particle1_pixels_v.resize(3);

    _image_particle0_plane0_tmp_x.clear();
    _image_particle0_plane0_tmp_y.clear();
    _image_particle0_plane0_tmp_v.clear();
    _image_particle0_plane1_tmp_x.clear();
    _image_particle0_plane1_tmp_y.clear();
    _image_particle0_plane1_tmp_v.clear();
    _image_particle0_plane2_tmp_x.clear();
    _image_particle0_plane2_tmp_y.clear();
    _image_particle0_plane2_tmp_v.clear();

    _image_particle1_plane0_tmp_x.clear();
    _image_particle1_plane0_tmp_y.clear();
    _image_particle1_plane0_tmp_v.clear();
    _image_particle1_plane1_tmp_x.clear();
    _image_particle1_plane1_tmp_y.clear();
    _image_particle1_plane1_tmp_v.clear();
    _image_particle1_plane2_tmp_x.clear();
    _image_particle1_plane2_tmp_y.clear();
    _image_particle1_plane2_tmp_v.clear();    
        
  }

  std::vector<float> dQdsAnalysis::dQdsDropper (std::vector<float> input_dqds){
    
    std::vector<float> res;
    res.clear();
    res = input_dqds;

    if (input_dqds.size()>_drop_location +5){
      res.clear();
      for (size_t idx = _drop_location; idx< input_dqds.size();++idx){
	res.push_back(input_dqds.at(idx));
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
  
  
  bool dQdsAnalysis::_PostProcess_() const
  { return true; }

  void dQdsAnalysis::_Process_() {

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
	    
	  auto end_pt = this_info3d.overall_pca_end_pt;
	  auto theta  = this_info3d.trunk_pca_theta;
	  auto phi    = this_info3d.trunk_pca_phi;
	  auto cosz   = std::cos(theta);
	  
	  cv::Mat masked_ctor;

	  masked_ctor = MaskImage(img_v.at(plane),par._ctor,0,false); 	
	  
	  data::AtomicContour thisatom;
	  data::AtomicContour raw_atom;
	  
	  thisatom = FindNonZero(masked_ctor);
	  raw_atom = FindNonZero(masked_ctor);
	  
	  par._atom = raw_atom;

	  //################For Adrien

	  _image_array_tmp.clear();
	  _image_array_tmp.resize(3);
	  
	  _image_array_tmp.at(plane).clear();
	  size_t img_size = par._atom.size();
	  /*if(img_size == 0) {
	    _image_array_tmp.at(plane).resize(1);
	    _image_array_tmp.at(plane).at(0).SetX(-1);
	    _image_array_tmp.at(plane).at(0).SetY(-1);
	    _image_array_tmp.at(plane).at(0).SetZ(-1);

	    }*/
	  _image_array_tmp.at(plane).resize(img_size);
	  
	  for (size_t idx = 0; idx < img_size; ++idx){
	    //std::vector<float> _image_array_tmp.at(plane).at(idx);
	    
	    //_image_array_tmp.at(plane).at(idx).clear();
	    _image_array_tmp.at(plane).at(idx).SetX(par._atom.at(idx).x);
	    _image_array_tmp.at(plane).at(idx).SetY(par._atom.at(idx).y);
	    assert(par._atom.at(idx).x >= 0);
	    assert(par._atom.at(idx).y >= 0);


	    assert(par._atom.at(idx).y < img_v.at(plane).rows);
	    assert(par._atom.at(idx).x < img_v.at(plane).cols);

	    _image_array_tmp.at(plane).at(idx).SetZ((int)(img_v.at(plane).at<uchar>(par._atom.at(idx).y, par._atom.at(idx).x)));
	    //this_img.push_back(_image_array_tmp.at(plane).at(idx));
	  }
	  
	  //#################For Adrien
	  float tot_q = 0;
	  
	  for(auto pt : thisatom) {
	    float q = (float)(masked_ctor.at<unsigned char>((size_t)(pt.y),(size_t)(pt.x)));
	    tot_q +=q;
	  }
	  
	  if (this_info2d.ptype == data::ParticleType_t::kTrack){
	    _trackp_totq = tot_q;
	    _trackp_cosz = cosz;
	  }
	  if (this_info2d.ptype == data::ParticleType_t::kShower){
	    _showerp_totq = tot_q;
	    _showerp_cosz = cosz;
	  }

	  geo2d::Circle<float> circle;
	  
	  circle = par._circle;

	  masked_ctor = MaskImage(masked_ctor,circle,0,false); 	
	  //masked_ctor = Threshold(masked_ctor, 10, 255);
	  
	  if ( FindNonZero(masked_ctor).size() <2 ) continue;
	  
	  thisatom = FindNonZero(masked_ctor);
	  
	  auto this_pca = CalcPCA(thisatom);
	  auto start_point = circle.center;
	  _vertex_v.at(plane).first  = start_point.x;
	  _vertex_v.at(plane).second = start_point.y;
	  geo2d::Vector<float> angle_scan_end_point;
	  
	  angle_scan_end_point = par._angle_scan_end_point;
	  
	  if (int(par._angle_scan_end_point.x == -9999)){
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
	      //std::cout<<"end_radius is "<<end_radius<<std::endl;
	    }
	  
	  end_circle.radius = end_radius;
	  masked_ctor = MaskImage(masked_ctor,end_circle,0,false);
	  auto end_pca = CalcPCA(thisatom);
	  
	  data::AtomicContour endatom;
	  endatom = FindNonZero(masked_ctor);
	  
	  std::vector<float> end_par_dqds;

	  geo2d::Vector<float> end_A;
	  geo2d::Vector<float> end_B;
	  
	  float a = 99999999,  b = 0;
	  
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

	  std::vector<float> trunk_dqds;
	  trunk_dqds.clear();
	  if (par_dqds.size() > 30){
	    for(size_t idx = 0; idx <30 ; ++idx ) trunk_dqds.push_back(par_dqds.at(idx));
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
	  //par._dqds_mean = 
	  auto mean_dqds = VectorMean(remove_dqds);
	  auto dqds_3dc = Correct3D(mean_dqds, theta, phi);
	  
	  //par._dqds_mean = dqds_3dc;
	  par._dqds_mean = mean_dqds;

	  if(pid == 0){
	    _dqds_0_v.at(plane) = mean_dqds;

	    _dqdx_0_v_3dc.at(plane) = dqds_3dc;//Correct3D(_dqds_0_v.at(plane), theta, phi);
	    _dqdx_0_end_v_3dc.at(plane) = end_dqds_3dc;
	    //_dqdx_0_v_3dc.at(plane) = Correct3D(truncated_dqds, _theta, _phi);
	    //_r_dqds_0_v.at(plane) = VectorMean(remove_dqds);
	    //_t_dqds_0_v.at(plane) = truncated_dqds;
	    _theta_0 = theta;
	    _phi_0   = phi;
	    length0  = length;
	    _particle0_end_point.at(plane).first  = atom_end_point.x;
	    _particle0_end_point.at(plane).second = atom_end_point.y;
	    _particle0_end_x = end_pt.at(0);
	    _particle0_end_y = end_pt.at(1);
	    _particle0_end_z = end_pt.at(2);

	    if(plane == 0){
	      for(size_t idx = 0; idx < _image_array_tmp.at(plane).size(); ++idx){
		  _image_particle0_plane0_tmp_x.push_back(_image_array_tmp.at(plane).at(idx).X());
		  _image_particle0_plane0_tmp_y.push_back(_image_array_tmp.at(plane).at(idx).Y());
		  _image_particle0_plane0_tmp_v.push_back(_image_array_tmp.at(plane).at(idx).Z());
	      }
	    }
	    if(plane == 1){
	      for(size_t idx = 0; idx < _image_array_tmp.at(plane).size(); ++idx){
		_image_particle0_plane1_tmp_x.push_back(_image_array_tmp.at(plane).at(idx).X());
		_image_particle0_plane1_tmp_y.push_back(_image_array_tmp.at(plane).at(idx).Y());
		_image_particle0_plane1_tmp_v.push_back(_image_array_tmp.at(plane).at(idx).Z());
	      }
	    }
	    if(plane == 2){
	      for(size_t idx = 0; idx < _image_array_tmp.at(plane).size(); ++idx){
		_image_particle0_plane2_tmp_x.push_back(_image_array_tmp.at(plane).at(idx).X());
		_image_particle0_plane2_tmp_y.push_back(_image_array_tmp.at(plane).at(idx).Y());
		_image_particle0_plane2_tmp_v.push_back(_image_array_tmp.at(plane).at(idx).Z());
	      }
	    }
	    _particle0_pixels_v.at(plane).clear();
	    _particle0_pixels_v.at(plane).resize(par._atom.size());
	    for(size_t idx = 0; idx < par._atom.size(); ++idx) {
	      _particle0_pixels_v.at(plane).at(idx).first   = par._atom.at(idx).x;
	      _particle0_pixels_v.at(plane).at(idx).second  = par._atom.at(idx).y;

	    }
	  }
	  if(pid == 1){
	    _dqds_1_v.at(plane) = mean_dqds;

	    _dqdx_1_v_3dc.at(plane) = dqds_3dc;//Correct3D(_dqds_1_v.at(plane), theta, phi);
	    _dqdx_1_end_v_3dc.at(plane) = end_dqds_3dc;
	    //_dqdx_1_v_3dc.at(plane) = Correct3D(truncated_dqds, _theta, _phi);
	    //_r_dqds_1_v.at(plane) = VectorMean(remove_dqds);
	    //_t_dqds_1_v.at(plane) = truncated_dqds;
	    _theta_1 = theta;
	    _phi_1   = phi;
	    length1  = length;
	    _particle1_end_point.at(plane).first  = atom_end_point.x;
	    _particle1_end_point.at(plane).second = atom_end_point.y;
	    _particle1_end_x = end_pt.at(0);
	    _particle1_end_y = end_pt.at(1);
	    _particle1_end_z = end_pt.at(2);	    
	    
	    if(plane == 0){
	      for(size_t idx = 0; idx < _image_array_tmp.at(plane).size(); ++idx){
		_image_particle1_plane0_tmp_x.push_back(_image_array_tmp.at(plane).at(idx).X());
		_image_particle1_plane0_tmp_y.push_back(_image_array_tmp.at(plane).at(idx).Y());
		_image_particle1_plane0_tmp_v.push_back(_image_array_tmp.at(plane).at(idx).Z());
	      }
	    }
	    if(plane == 1){
	      for(size_t idx = 0; idx < _image_array_tmp.at(plane).size(); ++idx){
		_image_particle1_plane1_tmp_x.push_back(_image_array_tmp.at(plane).at(idx).X());
		_image_particle1_plane1_tmp_y.push_back(_image_array_tmp.at(plane).at(idx).Y());
		_image_particle1_plane1_tmp_v.push_back(_image_array_tmp.at(plane).at(idx).Z());
	      }
	    }
	    if(plane == 2){
	      for(size_t idx = 0; idx < _image_array_tmp.at(plane).size(); ++idx){
		_image_particle1_plane2_tmp_x.push_back(_image_array_tmp.at(plane).at(idx).X());
		_image_particle1_plane2_tmp_y.push_back(_image_array_tmp.at(plane).at(idx).Y());
		_image_particle1_plane2_tmp_v.push_back(_image_array_tmp.at(plane).at(idx).Z());
	      }
	    }
	    _particle1_pixels_v.at(plane).clear();
	    _particle1_pixels_v.at(plane).resize(par._atom.size());
	    for(size_t idx = 0; idx < par._atom.size(); ++idx) {
	      _particle1_pixels_v.at(plane).at(idx).first   = par._atom.at(idx).x;
	      _particle1_pixels_v.at(plane).at(idx).second  = par._atom.at(idx).y;
	    }
	  }
	  
	  if (this_info2d.ptype == data::ParticleType_t::kTrack){
	    _trackp_dqds_v.at(plane)  = mean_dqds;//Correct3D(par._dqds_mean, theta, phi);
	    _trackp_dqdx_3dc_v.at(plane)  = dqds_3dc;//Correct3D(par._dqds_mean, theta, phi);
	  }
	  if (this_info2d.ptype == data::ParticleType_t::kShower){
	    _showerp_dqds_v.at(plane) = mean_dqds;//Correct3D(par._dqds_mean, theta, phi);
	    _showerp_dqdx_3dc_v.at(plane) = dqds_3dc;//Correct3D(par._dqds_mean, theta, phi);
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
	  }else{
	    _long_trackp_dqds_v.at(plane)  = _dqds_1_v.at(plane);
	    _short_trackp_dqds_v.at(plane) = _dqds_0_v.at(plane);
	    _long_trackp_dqdx_3dc_v.at(plane)  = _dqdx_1_v_3dc.at(plane);
	    _short_trackp_dqdx_3dc_v.at(plane) = _dqdx_0_v_3dc.at(plane);
	  } 
	}
	
	if(_dqds_0_v.at(plane)     == 0 ) _dqds_0_v.at(plane)     = _dqds_1_v.at(plane); //stupid way for cases where dqds not calculated
	if(_dqds_1_v.at(plane)     == 0 ) _dqds_1_v.at(plane)     = _dqds_0_v.at(plane); //stupid way for cases where dqds not calculated
	//if(_t_dqds_0_v.at(plane)   == 0 ) _t_dqds_0_v.at(plane)   = _t_dqds_1_v.at(plane); //stupid way for cases where dqds not calculated
	//if(_t_dqds_1_v.at(plane)   == 0 ) _t_dqds_1_v.at(plane)   = _t_dqds_0_v.at(plane); //stupid way for cases where dqds not calculated
	//if(_r_dqds_0_v.at(plane)   == 0 ) _r_dqds_0_v.at(plane)   = _r_dqds_1_v.at(plane); //stupid way for cases where dqds not calculated
	//if(_r_dqds_1_v.at(plane)   == 0 ) _r_dqds_1_v.at(plane)   = _r_dqds_0_v.at(plane); //stupid way for cases where dqds not calculated
	if(_dqdx_0_v_3dc.at(plane) == 0 ) _dqdx_0_v_3dc.at(plane) = _dqdx_1_v_3dc.at(plane); //stupid way for cases where dqds not calculated
	if(_dqdx_1_v_3dc.at(plane) == 0 ) _dqdx_1_v_3dc.at(plane) = _dqdx_0_v_3dc.at(plane); //stupid way for cases where dqds not calculated
	if(_dqdx_0_end_v_3dc.at(plane) == 0 ) _dqdx_0_end_v_3dc.at(plane) = _dqdx_1_end_v_3dc.at(plane); //stupid way for cases where dqds not calculated
	if(_dqdx_1_end_v_3dc.at(plane) == 0 ) _dqdx_1_end_v_3dc.at(plane) = _dqdx_0_end_v_3dc.at(plane); //stupid way for cases where dqds not calculated


	_dqds_diff_v.at(plane)     = std::abs(_dqds_0_v.at(plane)     -  _dqds_1_v.at(plane));
	//_t_dqds_diff_v.at(plane)   = std::abs(_t_dqds_0_v.at(plane)   -  _t_dqds_1_v.at(plane));
	//_r_dqds_diff_v.at(plane)   = std::abs(_r_dqds_0_v.at(plane)   -  _r_dqds_1_v.at(plane));
	_dqdx_diff_v_3dc.at(plane) = std::abs(_dqdx_0_v_3dc.at(plane) -  _dqdx_1_v_3dc.at(plane));
	
	_dqds_ratio_v.at(plane)     = (_dqds_0_v.at(plane)/_dqds_1_v.at(plane) <= 1 ? 
				    _dqds_0_v.at(plane)/_dqds_1_v.at(plane) : _dqds_1_v.at(plane)/_dqds_0_v.at(plane) );
	//_t_dqds_ratio_v.at(plane)   = (_t_dqds_0_v.at(plane)/_t_dqds_1_v.at(plane) <= 1 ? 
	//_t_dqds_0_v.at(plane)/_t_dqds_1_v.at(plane) : _t_dqds_1_v.at(plane)/_t_dqds_0_v.at(plane) );
	//_r_dqds_ratio_v.at(plane)   = (_r_dqds_0_v.at(plane)/_r_dqds_1_v.at(plane) <= 1 ? 
	//_r_dqds_0_v.at(plane)/_r_dqds_1_v.at(plane) : _r_dqds_1_v.at(plane)/_r_dqds_0_v.at(plane) );
	_dqdx_ratio_v_3dc.at(plane) = (_dqdx_0_v_3dc.at(plane)/_dqdx_1_v_3dc.at(plane) <= 1 ?
				    _dqdx_0_v_3dc.at(plane)/_dqdx_1_v_3dc.at(plane) : _dqdx_1_v_3dc.at(plane)/_dqdx_0_v_3dc.at(plane) );
	
      }
     
      //if (_dqds_diff_v.at(0) >= 0 )_dqds_diff_01 = _dqds_diff_v.at(0);
      //if (_dqds_diff_v.at(1) > _dqds_diff_01 && _dqds_diff_v.at(1)>0) _dqds_diff_01 = _dqds_diff_v.at(1);
      //if (_dqds_ratio_v.at(0) >= 0 )_dqds_ratio_01 = _dqds_ratio_v.at(0);
      //if (_dqds_ratio_v.at(1)< _dqds_ratio_01 && _dqds_ratio_v.at(1)>0) _dqds_ratio_01 = _dqds_ratio_v.at(1);
      if (!_dqds_diff_v.empty()) {
	auto res = Sort01(_dqds_diff_v);
	if (!res.empty())
	  _dqds_diff_01 = res.at(1);
	else
	  _dqds_diff_01=kINVALID_DOUBLE;
      }else
	_dqds_diff_01=kINVALID_DOUBLE;
      

      if (!_dqds_ratio_v.empty()) {
	auto res = Sort01(_dqds_ratio_v);
	if (!res.empty())
	  _dqds_ratio_01 =  res.at(0);
	else
	  _dqds_ratio_01 = kINVALID_DOUBLE;
      }else
	_dqds_ratio_01=kINVALID_DOUBLE;
      
      //_t_dqds_diff_01    = Sort01(_t_dqds_diff_v).at(1);
      //_t_dqds_ratio_01   = Sort01(_t_dqds_ratio_v).at(0);
      //_r_dqds_diff_01    = Sort01(_r_dqds_diff_v).at(1);
      //_r_dqds_ratio_01   = Sort01(_r_dqds_ratio_v).at(0);

      if (!_dqdx_diff_v_3dc.empty()) {
	auto res = Sort01(_dqdx_diff_v_3dc);
	if (!res.empty())
	  _dqdx_diff_01_3dc  = res.at(1);
	else
	  _dqdx_diff_01_3dc = kINVALID_DOUBLE;
      }else
	_dqdx_diff_01_3dc = kINVALID_DOUBLE;

      if (!_dqdx_ratio_v_3dc.empty()) {
	auto res = Sort01(_dqdx_ratio_v_3dc);
	if (!res.empty())
	  _dqdx_ratio_01_3dc = res.at(0);
	else
	  _dqdx_ratio_01_3dc = kINVALID_DOUBLE;
      } else
	_dqdx_ratio_01_3dc = kINVALID_DOUBLE;
      
      /*
      auto minmax_diff = std::minmax_element(std::begin(_dqds_diff_v), std::end(_dqds_diff_v));
      _dqds_diff_01 = *(minmax_diff.second);
      auto minmax_ratio = std::minmax_element(std::begin(_dqds_ratio_v), std::end(_dqds_ratio_v));
      _dqds_ratio_01 = *(minmax_ratio.first);
      auto minmax_diff_3dc = std::minmax_element(std::begin(_dqdx_diff_v_3dc), std::end(_dqdx_diff_v_3dc));
      _dqdx_diff_01_3dc = *(minmax_diff_3dc.second);
      auto minmax_ratio_3dc = std::minmax_element(std::begin(_dqdx_ratio_v_3dc), std::end(_dqdx_ratio_v_3dc));
      _dqdx_ratio_01_3dc = *(minmax_ratio_3dc.first);
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
