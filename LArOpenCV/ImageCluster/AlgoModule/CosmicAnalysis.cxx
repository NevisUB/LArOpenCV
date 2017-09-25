#ifndef __COSMICANALYSIS_CXX__
#define __COSMICANALYSIS_CXX__

#include "CosmicAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  static CosmicAnalysisFactory __global_CosmicAnalysisFactory__;

  void CosmicAnalysis::_Configure_(const Config_t &pset)
  {

    auto name_combined = pset.get<std::string>("VertexProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "VertexProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    auto name_particle = pset.get<std::string>("ParticleProducer");
    _particle_id=kINVALID_ALGO_ID;
    if (!name_particle.empty()) {
      _particle_id = this->ID(name_particle);
      if (_particle_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "ParticleProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _shape_analysis_algo_id = kINVALID_ALGO_ID;
    auto shape_analysis_algo_name = pset.get<std::string>("ShapeAnalysisAlgo","");
    if (!shape_analysis_algo_name.empty()) {
      _shape_analysis_algo_id = this->ID(shape_analysis_algo_name);
      if (_shape_analysis_algo_id==kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "ShapeAnalysisAlgo ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _tree = new TTree("CosmicAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"        , &_roid      , "roid/I");
    _tree->Branch("vtxid"       , &_vtxid     , "vtxid/I");
    _tree->Branch("x" , &_x , "x/D");
    _tree->Branch("y" , &_y , "y/D");
    _tree->Branch("z" , &_z , "z/D");
    
    _tree->Branch("dtheta_sum_p0",&_dtheta_sum_p0,"dtheta_sum_p0/F");
    _tree->Branch("dtheta_sum_p1",&_dtheta_sum_p1,"dtheta_sum_p1/F");
    _tree->Branch("dtheta_sum_p2",&_dtheta_sum_p2,"dtheta_sum_p2/F");

    _tree->Branch("dtheta_mean_p0",&_dtheta_mean_p0,"dtheta_mean_p0/F");
    _tree->Branch("dtheta_mean_p1",&_dtheta_mean_p1,"dtheta_mean_p1/F");
    _tree->Branch("dtheta_mean_p2",&_dtheta_mean_p2,"dtheta_mean_p2/F");
  
    _tree->Branch("dtheta_xs_p0",&_dtheta_xs_p0,"dtheta_xs_p0/F");
    _tree->Branch("dtheta_xs_p1",&_dtheta_xs_p1,"dtheta_xs_p1/F");
    _tree->Branch("dtheta_xs_p2",&_dtheta_xs_p2,"dtheta_xs_p2/F");

  
    _tree->Branch("charge_asym_v" , &_charge_asym_v);
    _tree->Branch("npixel_asym_v" , &_npixel_asym_v);
    _tree->Branch("ctor_asym_v"   , &_ctor_asym_v);
    _tree->Branch("area_asym_v"   , &_area_asym_v);
    _tree->Branch("frac_stopmu_v" , &_frac_stopmu_v);
    _tree->Branch("frac_thrumu_v" , &_frac_thrumu_v);

    _tree->Branch("charge_asym_min_v" , &_charge_asym_min_v);
    _tree->Branch("npixel_asym_min_v" , &_npixel_asym_min_v);
    _tree->Branch("ctor_asym_min_v"   , &_ctor_asym_min_v);
    _tree->Branch("area_asym_min_v"   , &_area_asym_min_v);

    _tree->Branch("charge_asym_max_v" , &_charge_asym_max_v);
    _tree->Branch("npixel_asym_max_v" , &_npixel_asym_max_v);
    _tree->Branch("ctor_asym_max_v"   , &_ctor_asym_max_v);
    _tree->Branch("area_asym_max_v"   , &_area_asym_max_v);

    _tree->Branch("hull_area_overlap_avg" , &_hull_area_overlap_avg, "hull_area_overlap_avg/F");
    _tree->Branch("hull_area_overlap_min" , &_hull_area_overlap_min, "hull_area_overlap_min/F");
    _tree->Branch("hull_area_overlap_max" , &_hull_area_overlap_max, "hull_area_overlap_max/F");
    _tree->Branch("hull_nplanes", &_hull_nplanes, "hull_nplanes/I");

    _tree->Branch("shape_match_I1_v",&_shape_match_I1_v);
    _tree->Branch("shape_match_I2_v",&_shape_match_I2_v);
    _tree->Branch("shape_match_I3_v",&_shape_match_I3_v);
    
    _roid = 0;
  }

  void CosmicAnalysis::_Process_() {
    
    LAROCV_INFO() << "start" << std::endl;

    if(NextEvent()) _roid=0;

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);

    auto thrumu_img_v = ImageArray(ImageSetID_t::kImageSetThruMu);
    auto stopmu_img_v = ImageArray(ImageSetID_t::kImageSetStopMu);

    const auto& meta_v = MetaArray();

    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();

    const auto& info2d_data = AlgoData<data::Info2DArray>(_shape_analysis_algo_id,0);
    const auto& info2d_v = info2d_data.as_vector();
    
    _vtxid = -1;

    LAROCV_DEBUG() << "Got " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      ClearVertex();

      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;
      
      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      LAROCV_DEBUG() << "Got " << par_id_v.size() << " particles" << std::endl;
      LAROCV_DEBUG() << " & algo data particle vector sz " << particle_v.size() << std::endl;

      ResetVectors(par_id_v.size());
      
      for(size_t plane=0; plane<3; ++plane) {
	const auto& cvtx = vtx3d.cvtx2d_v.at(plane);
	if (cvtx.xs_v.empty()) continue;
	if (plane==0) { 
	  _dtheta_sum_p0  = cvtx.sum_dtheta(); 
	  _dtheta_mean_p0 = cvtx.mean_dtheta(); 
	  _dtheta_xs_p0   = cvtx.dtheta_xs;
	}

	else if (plane==1) { 
	  _dtheta_sum_p1  = cvtx.sum_dtheta(); 
	  _dtheta_mean_p1 = cvtx.mean_dtheta(); 
	  _dtheta_xs_p1   = cvtx.dtheta_xs;
	}
	
	else if (plane==2) { 
	  _dtheta_sum_p2  = cvtx.sum_dtheta(); 
	  _dtheta_mean_p2 = cvtx.mean_dtheta(); 
	  _dtheta_xs_p2   = cvtx.dtheta_xs;
	}
      }

      //
      // Store the hull contour
      //
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];
	
	const auto info2d_id = ass_man.GetOneAss(par,info2d_data.ID());
	const auto& info2d = info2d_data.as_vector().at(info2d_id);
	
	LAROCV_DEBUG() << "At particle id " << par_idx << " @ " << par_id << std::endl;
	
	// loop over plane to average
	float nplanes = 0.0;

	float thrumu_nonzero_avg = 0.0;
	float thrumu_nonzero_max = -1*kINVALID_FLOAT;
	float thrumu_nonzero_min = kINVALID_FLOAT;

	float stopmu_nonzero_avg = 0.0;
	float stopmu_nonzero_max = -1*kINVALID_FLOAT;
	float stopmu_nonzero_min = kINVALID_FLOAT;

	float ctor_avg = 0.0;
	float ctor_max = -1*kINVALID_FLOAT;
	float ctor_min = kINVALID_FLOAT;
	
	for(size_t plane=0; plane<3; ++plane) {
	  const auto& ctor = par._par_v[plane]._ctor;
	  if ( ctor.empty() ) continue;
	  nplanes += 1.0;
	
	  //
	  // tagger count
	  //
	  float thrumu_nonzero = (float)CountNonZero(MaskImage(thrumu_img_v[plane],ctor,0,false));
	  float stopmu_nonzero = (float)CountNonZero(MaskImage(stopmu_img_v[plane],ctor,0,false));
  
	  thrumu_nonzero_avg += thrumu_nonzero;
	  stopmu_nonzero_avg += stopmu_nonzero;

	  if (thrumu_nonzero < thrumu_nonzero_min) thrumu_nonzero_min = thrumu_nonzero;
	  if (thrumu_nonzero > thrumu_nonzero_max) thrumu_nonzero_max = thrumu_nonzero;

	  if (stopmu_nonzero < stopmu_nonzero_min) stopmu_nonzero_min = stopmu_nonzero;
	  if (stopmu_nonzero > stopmu_nonzero_max) stopmu_nonzero_max = stopmu_nonzero;
	  
	  //
	  // ctor 
	  //
	  float ctor_sz = float(ctor.size());
	  ctor_avg += ctor_sz;

	  if (ctor_sz > ctor_max) ctor_max = ctor_sz;
	  if (ctor_sz < ctor_min) ctor_min = ctor_sz;
	  
	}

	//assert(info2d.nplanes == (int) nplanes);
	thrumu_nonzero_avg = thrumu_nonzero_avg / info2d.area / nplanes;
	stopmu_nonzero_avg = stopmu_nonzero_avg / info2d.area / nplanes;
	
	auto& charge_asym = _charge_asym_v[par_idx];
	auto& npixel_asym = _npixel_asym_v[par_idx];
	auto& ctor_asym   = _ctor_asym_v[par_idx];
	auto& area_asym   = _area_asym_v[par_idx];

	if (info2d.qsum_min != info2d.qsum_max)
	  charge_asym = (info2d.qsum_max   - info2d.qsum_min)   / (info2d.qsum_max   + info2d.qsum_min);

	if(info2d.npixel_min != info2d.npixel_max)
	  npixel_asym = (info2d.npixel_max - info2d.npixel_min) / (info2d.npixel_max + info2d.npixel_min);

	if(info2d.area_min != info2d.area_max)
	  area_asym   = (info2d.area_max   - info2d.area_min)   / (info2d.area_max   + info2d.area_min);	  

	if(ctor_min != ctor_max)
	  ctor_asym   = (ctor_max   - ctor_min)   / (ctor_max   + ctor_min);

	auto& frac_stopmu = _frac_stopmu_v[par_idx];
	auto& frac_thrumu = _frac_thrumu_v[par_idx];
	
	frac_stopmu = stopmu_nonzero_avg;
	frac_thrumu = thrumu_nonzero_avg;
	
      }

      
      //
      // Two particle hull overlap
      //
      size_t par_id1 = par_id_v.front();
      size_t par_id2 = par_id_v.back();

      const auto& par1 = particle_v.at(par_id1);
      const auto& par2 = particle_v.at(par_id2);

      _hull_area_overlap_avg =    0.0;
      _hull_area_overlap_min =    kINVALID_FLOAT;
      _hull_area_overlap_max = -1*kINVALID_FLOAT;
      
      _hull_nplanes = 0;
      for(size_t plane=0; plane<3; ++plane) {
	const auto& ctor1 = par1._par_v[plane]._ctor;
	const auto& ctor2 = par2._par_v[plane]._ctor;
	
	if (ctor1.empty() || ctor2.empty()) continue;
	_hull_nplanes += 1;

	auto hull_ctor1 = ConvexHull(ctor1);
	auto hull_ctor2 = ConvexHull(ctor2);
	
	float hull_area1 = (float)ContourArea(thresh_img_v[plane],hull_ctor1);
	float hull_area2 = (float)ContourArea(thresh_img_v[plane],hull_ctor2);

	float area_overlap = AreaOverlap(hull_ctor1,hull_ctor2);

	auto hull_ratio = hull_area1 > hull_area2 ? area_overlap / hull_area2 : area_overlap / hull_area1;

	_hull_area_overlap_avg += hull_ratio;
	
	if (hull_ratio > _hull_area_overlap_max) _hull_area_overlap_max = hull_ratio;
	if (hull_ratio < _hull_area_overlap_min) _hull_area_overlap_min = hull_ratio;
	
      }
      _hull_area_overlap_avg /= (float) _hull_nplanes;

      //
      // Two per particle pair cv::ShapeMatch
      //
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id     = par_id_v[par_idx];
	const auto& par = particle_v[par_id];
		
	// calculate 3 combinations, take the lowest of each 3?
	std::array<std::pair<size_t,size_t>,3> comb_v;
	comb_v[0] = std::make_pair(0,1);
	comb_v[1] = std::make_pair(0,2);
	comb_v[2] = std::make_pair(1,2);

	double sum_shape_match = kINVALID_DOUBLE;
	for(auto comb : comb_v) {
	  const auto& ctor1 = par._par_v.at(comb.first)._ctor;
	  const auto& ctor2 = par._par_v.at(comb.second)._ctor;

	  if (ctor1.empty()) continue;
	  if (ctor2.empty()) continue;

	  auto shape_match_I1 = cv::matchShapes(ctor1,ctor2,CV_CONTOURS_MATCH_I1,0.0);
	  auto shape_match_I2 = cv::matchShapes(ctor1,ctor2,CV_CONTOURS_MATCH_I2,0.0);
	  auto shape_match_I3 = cv::matchShapes(ctor1,ctor2,CV_CONTOURS_MATCH_I3,0.0);

	  auto total_shape_match = shape_match_I1 + shape_match_I2 + shape_match_I3;
	  std::cout << "total_shape_match : " << total_shape_match << std::endl;
	  if (total_shape_match > sum_shape_match) continue;
	  sum_shape_match = total_shape_match;

	  _shape_match_I1_v[par_idx] = shape_match_I1;
	  _shape_match_I2_v[par_idx] = shape_match_I2;
	  _shape_match_I3_v[par_idx] = shape_match_I3;
	}
      } // end this particle
      _tree->Fill();
    } // end this vertex
      
    _roid += 1;
    LAROCV_INFO() << "end" << std::endl;
  }

  void CosmicAnalysis::ResetVectors(size_t sz) {

    _charge_asym_v.resize(sz,kINVALID_FLOAT);
    _npixel_asym_v.resize(sz,kINVALID_FLOAT);
    _ctor_asym_v.resize(sz,kINVALID_FLOAT);
    _area_asym_v.resize(sz,kINVALID_FLOAT);
    _frac_stopmu_v.resize(sz,kINVALID_FLOAT);
    _frac_thrumu_v.resize(sz,kINVALID_FLOAT);

    _charge_asym_min_v.resize(sz,kINVALID_FLOAT);
    _npixel_asym_min_v.resize(sz,kINVALID_FLOAT);
    _ctor_asym_min_v.resize(sz,kINVALID_FLOAT);
    _area_asym_min_v.resize(sz,kINVALID_FLOAT);
    _charge_asym_max_v.resize(sz,kINVALID_FLOAT);
    _npixel_asym_max_v.resize(sz,kINVALID_FLOAT);
    _ctor_asym_max_v.resize(sz,kINVALID_FLOAT);
    _area_asym_max_v.resize(sz,kINVALID_FLOAT);
    
    _hull_area_overlap_avg = kINVALID_FLOAT;
    _hull_area_overlap_min = kINVALID_FLOAT;
    _hull_area_overlap_max = kINVALID_FLOAT;

    _shape_match_I1_v.resize(sz,kINVALID_DOUBLE);
    _shape_match_I2_v.resize(sz,kINVALID_DOUBLE);
    _shape_match_I3_v.resize(sz,kINVALID_DOUBLE);

  }

  void CosmicAnalysis::ClearVertex() {
    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

    _charge_asym_v.clear();
    _npixel_asym_v.clear();
    _ctor_asym_v.clear();
    _area_asym_v.clear();
    _frac_stopmu_v.clear();
    _frac_thrumu_v.clear();

    _charge_asym_min_v.clear();
    _npixel_asym_min_v.clear();
    _ctor_asym_min_v.clear();
    _area_asym_min_v.clear();

    _charge_asym_max_v.clear();
    _npixel_asym_max_v.clear();
    _ctor_asym_max_v.clear();
    _area_asym_max_v.clear();
    
    _hull_area_overlap_avg = kINVALID_FLOAT;
    _hull_area_overlap_min = kINVALID_FLOAT;
    _hull_area_overlap_max = kINVALID_FLOAT;
    _hull_nplanes = kINVALID_INT;

    _shape_match_I1_v.clear();
    _shape_match_I2_v.clear();
    _shape_match_I3_v.clear();

    _dtheta_sum_p0 = kINVALID_FLOAT;
    _dtheta_sum_p1 = kINVALID_FLOAT;
    _dtheta_sum_p2 = kINVALID_FLOAT;

    _dtheta_mean_p0 = kINVALID_FLOAT;
    _dtheta_mean_p1 = kINVALID_FLOAT;
    _dtheta_mean_p2 = kINVALID_FLOAT;

    _dtheta_xs_p0 = kINVALID_FLOAT;
    _dtheta_xs_p1 = kINVALID_FLOAT;
    _dtheta_xs_p2 = kINVALID_FLOAT;


  }

}
#endif
