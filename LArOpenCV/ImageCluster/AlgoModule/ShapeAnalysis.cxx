#ifndef __SHAPEANALYSIS_CXX__
#define __SHAPEANALYSIS_CXX__

#include "ShapeAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelChunk.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"
#include "Geo2D/Core/Geo2D.h"

namespace larocv {
  
  /// Global larocv::ShapeAnalysisFactory to register AlgoFactory
  static ShapeAnalysisFactory __global_ShapeAnalysisFactory__;
  
  void ShapeAnalysis::_Configure_(const Config_t &pset)
  {
    auto name_combined = pset.get<std::string>("VertexProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
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

    _tree = new TTree("ShapeAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"       , &_roid      , "roid/I");
    _tree->Branch("vtxid"      , &_vtxid     , "vtxid/I");
    _tree->Branch("x"          , &_x         , "x/D");
    _tree->Branch("y"          , &_y         , "y/D");
    _tree->Branch("z"          , &_z         , "z/D");
    _tree->Branch("nparticles" , &_nparticles, "nparticles/I");

    _tree->Branch("nplanes_v"         , &_nplanes_v);

    _tree->Branch("length_v"          , &_length_v);
    _tree->Branch("width_v"           , &_width_v);
    _tree->Branch("perimeter_v"       , &_perimeter_v);
    _tree->Branch("area_v"            , &_area_v);
    _tree->Branch("npixel_v"          , &_npixel_v);
    _tree->Branch("qsum_v"            , &_qsum_v);  
    _tree->Branch("track_frac_v"      , &_track_frac_v);
    _tree->Branch("shower_frac_v"     , &_shower_frac_v);
    _tree->Branch("mean_pixel_dist_v" , &_mean_pixel_dist_v);
    _tree->Branch("sigma_pixel_dist_v", &_sigma_pixel_dist_v);
    _tree->Branch("angular_sum_v"     , &_angular_sum_v);
    _tree->Branch("triangle_d_sum_v"  , &_triangle_d_sum_v);

    _tree->Branch("length_max_v"          , &_length_max_v);
    _tree->Branch("width_max_v"           , &_width_max_v);
    _tree->Branch("perimeter_max_v"       , &_perimeter_max_v);
    _tree->Branch("area_max_v"            , &_area_max_v);
    _tree->Branch("npixel_max_v"          , &_npixel_max_v);
    _tree->Branch("qsum_max_v"            , &_qsum_max_v);
    _tree->Branch("track_frac_max_v"      , &_track_frac_max_v);
    _tree->Branch("shower_frac_max_v"     , &_shower_frac_max_v);
    _tree->Branch("mean_pixel_dist_max_v" , &_mean_pixel_dist_max_v);
    _tree->Branch("sigma_pixel_dist_max_v", &_sigma_pixel_dist_max_v);
    _tree->Branch("angular_sum_max_v"     , &_angular_sum_max_v);
    _tree->Branch("triangle_d_max_v"      , &_triangle_d_max_v);

    _tree->Branch("length_min_v"           , &_length_min_v);
    _tree->Branch("width_min_v"            , &_width_min_v);
    _tree->Branch("perimeter_min_v"        , &_perimeter_min_v);
    _tree->Branch("area_min_v"             , &_area_min_v);
    _tree->Branch("npixel_min_v"           , &_npixel_min_v);
    _tree->Branch("qsum_min_v"             , &_qsum_min_v);
    _tree->Branch("track_frac_min_v"       , &_track_frac_min_v);
    _tree->Branch("shower_frac_min_v"      , &_shower_frac_min_v);
    _tree->Branch("mean_pixel_dist_min_v"  , &_mean_pixel_dist_min_v);
    _tree->Branch("sigma_pixel_dist_min_v" , &_sigma_pixel_dist_min_v);
    _tree->Branch("angular_sum_min_v"      , &_angular_sum_min_v);
    _tree->Branch("triangle_d_min_v"       , &_triangle_d_min_v);
     
    _tree->Branch("track_par_max_id"   , &_track_par_max_id   , "track_par_max_id/I");
    _tree->Branch("shower_par_max_id"  , &_shower_par_max_id  , "shower_par_max_id/I");
    _tree->Branch("track_par_max_frac" , &_track_par_max_frac , "track_par_max_frac/F");
    _tree->Branch("shower_par_max_frac", &_shower_par_max_frac, "shower_par_max_frac/F");

    _tree->Branch("par1_type", &_par1_type, "par1_type/I");
    _tree->Branch("par2_type", &_par2_type, "par2_type/I");
    _tree->Branch("par1_frac", &_par1_frac, "par1_frac/F");
    _tree->Branch("par2_frac", &_par2_frac, "par2_frac/F");

    _tree->Branch("vertex_type",&_vertex_type,"vertex_type/I");
    
    _roid = 0;
    
    Register(new data::Info2DArray);    
  }

  
  void ShapeAnalysis::_Process_() {

    ClearEvent();
    
    if(NextEvent()) _roid=0;
    
    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v     = ImageArray(ImageSetID_t::kImageSetWire);
    auto track_img_v   = ImageArray(ImageSetID_t::kImageSetTrack);
    auto shower_img_v  = ImageArray(ImageSetID_t::kImageSetShower);

    // Get Info2D to store particle type
    auto& info2d_arr = AlgoData<data::Info2DArray>(0);
    
    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();

    _vtxid = -1;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {

      ClearVertex();

      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vertex_type = (int) vtx3d.type;
      
      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      _vtxid += 1;

      std::vector<const data::Particle*> par_v;

      for(auto par_id : par_id_v) {
	const auto& par = particle_v[par_id];
	par_v.push_back(&par);
      }

      for(const auto par_ptr : par_v) {
	const auto& par = *par_ptr;
	
	float length    = 0.0;
	float width     = 0.0;
	float perimeter = 0.0;
	float area  = 0.0;
	float npixel = 0.0;
	float qsum = 0.0;
	float track_frac = 0.0;
	float shower_frac = 0.0;
	float mean_pixel_dist = 0.0;
	float sigma_pixel_dist = 0.0;
	float angular_sum = 0.0;
	float triangle_d_sum = 0.0;

	float width_max            = -1.0*kINVALID_FLOAT;
	float length_max           = -1.0*kINVALID_FLOAT;
	float perimeter_max        = -1.0*kINVALID_FLOAT;
	float area_max             = -1.0*kINVALID_FLOAT;
	float npixel_max           = -1.0*kINVALID_FLOAT;
	float qsum_max             = -1.0*kINVALID_FLOAT;
	float track_frac_max       = -1.0*kINVALID_FLOAT;
	float shower_frac_max      = -1.0*kINVALID_FLOAT;
	float mean_pixel_dist_max  = -1.0*kINVALID_FLOAT;
	float sigma_pixel_dist_max = -1.0*kINVALID_FLOAT;
	float angular_sum_max      = -1.0*kINVALID_FLOAT;
	float triangle_d_max       = -1.0*kINVALID_FLOAT;

	float length_min           =  1.0*kINVALID_FLOAT;
	float width_min            =  1.0*kINVALID_FLOAT;
	float perimeter_min        =  1.0*kINVALID_FLOAT;
	float area_min             =  1.0*kINVALID_FLOAT;
	float npixel_min           =  1.0*kINVALID_FLOAT;
	float qsum_min             =  1.0*kINVALID_FLOAT;
	float track_frac_min       =  1.0*kINVALID_FLOAT;
	float shower_frac_min      =  1.0*kINVALID_FLOAT;
	float mean_pixel_dist_min  =  1.0*kINVALID_FLOAT;
	float sigma_pixel_dist_min =  1.0*kINVALID_FLOAT;
	float angular_sum_min      =  1.0*kINVALID_FLOAT;
	float triangle_d_min       =  1.0*kINVALID_FLOAT;
	
	int nplanes = 0;
	for(size_t plane=0; plane<3; ++plane) {

	  const auto& pcluster = par._par_v[plane];
	  const auto& pctor = pcluster._ctor;
	  if (pctor.empty()) continue;

	  nplanes += 1;
	  
	  auto& adc_img    = adc_img_v[plane];
	  auto& track_img  = track_img_v[plane];
	  auto& shower_img = shower_img_v[plane];
	    
	  PixelChunk pchunk(pctor,adc_img,track_img,shower_img);
	  length           += pchunk.length;
	  width            += pchunk.width;
	  perimeter        += pchunk.perimeter;
	  area             += pchunk.area;
	  npixel           += pchunk.npixel;
	  qsum             += pchunk.qsum;
	  track_frac       += pchunk.track_frac;
	  shower_frac      += pchunk.shower_frac;
	  mean_pixel_dist  += pchunk.mean_pixel_dist;
	  sigma_pixel_dist += pchunk.sigma_pixel_dist;
	  angular_sum      += pchunk.angular_sum;

	  // Estimate linearity from 3 point triangle
	  const auto& vtx2d = vtx3d.vtx2d_v.at(plane).pt;
	  auto valid_end = pchunk.EstimateStartEndPixel(vtx2d,adc_img,10);
	  if (!valid_end) LAROCV_WARNING() << "Invalid start or end point" << std::endl;
	  auto valid_trunk = pchunk.EstimateTrunkPCA(vtx2d,adc_img,15,10);
	  if (!valid_trunk) LAROCV_WARNING() << "Invalid trunk PCA determined" << std::endl;

	  geo2d::Vector<float> ret1,ret2;
	  float dist = geo2d::ClosestPoint(pchunk.trunkPCA,pchunk.end_pt,ret1,ret2);
	  if (std::isinf(dist))
	    dist = kINVALID_FLOAT;

	  triangle_d_sum += dist;

	  if (dist > triangle_d_max && dist != kINVALID_FLOAT)
	    triangle_d_max = dist;

	  if (dist < triangle_d_min && dist != kINVALID_FLOAT)
	    triangle_d_min = dist;

	  if (pchunk.length      > length_max)      length_max       = pchunk.length;
	  if (pchunk.width       > width_max)       width_max        = pchunk.width;
	  if (pchunk.perimeter   > perimeter_max)   perimeter_max    = pchunk.perimeter;
	  if (pchunk.area        > area_max)        area_max         = pchunk.area;
	  if (pchunk.npixel      > npixel_max)      npixel_max       = pchunk.npixel;
	  if (pchunk.qsum        > qsum_max)        qsum_max         = pchunk.qsum;
	  if (pchunk.track_frac  > track_frac_max)  track_frac_max   = pchunk.track_frac;
	  if (pchunk.shower_frac > shower_frac_max) shower_frac_max  = pchunk.shower_frac;

	  if (pchunk.mean_pixel_dist  > mean_pixel_dist_max)  mean_pixel_dist_max  = pchunk.mean_pixel_dist;
	  if (pchunk.sigma_pixel_dist > sigma_pixel_dist_max) sigma_pixel_dist_max = pchunk.sigma_pixel_dist;


	  if (pchunk.length      < length_min)      length_min       = pchunk.length;
	  if (pchunk.width       < width_min)       width_min        = pchunk.width;
	  if (pchunk.perimeter   < perimeter_min)   perimeter_min    = pchunk.perimeter;
	  if (pchunk.area        < area_min)        area_min         = pchunk.area;
	  if (pchunk.npixel      < npixel_min)      npixel_min       = pchunk.npixel;
	  if (pchunk.qsum        < qsum_min)        qsum_min         = pchunk.qsum;
	  if (pchunk.track_frac  < track_frac_min)  track_frac_min   = pchunk.track_frac;
	  if (pchunk.shower_frac < shower_frac_min) shower_frac_min  = pchunk.shower_frac;

	  if (pchunk.mean_pixel_dist  < mean_pixel_dist_min)  mean_pixel_dist_min  = pchunk.mean_pixel_dist;
	  if (pchunk.sigma_pixel_dist < sigma_pixel_dist_min) sigma_pixel_dist_min = pchunk.sigma_pixel_dist;
	  
	  
	  
	} // end plane
	
	_length_v           . push_back(length);
	_width_v            . push_back(width);
	_perimeter_v        . push_back(perimeter);
	_area_v             . push_back(area);
	_npixel_v           . push_back(npixel);
	_qsum_v             . push_back(qsum);
	_track_frac_v       . push_back(track_frac);
	_shower_frac_v      . push_back(shower_frac);
	_mean_pixel_dist_v  . push_back(mean_pixel_dist);
	_sigma_pixel_dist_v . push_back(sigma_pixel_dist);
	_angular_sum_v      . push_back(angular_sum);
	_triangle_d_sum_v   . push_back(triangle_d_sum);

	_length_min_v           . push_back(length_min);
	_width_min_v            . push_back(width_min);
	_perimeter_min_v        . push_back(perimeter_min);
	_area_min_v             . push_back(area_min);
	_npixel_min_v           . push_back(npixel_min);
	_qsum_min_v             . push_back(qsum_min);
	_track_frac_min_v       . push_back(track_frac_min);
	_shower_frac_min_v      . push_back(shower_frac_min);
	_mean_pixel_dist_min_v  . push_back(mean_pixel_dist_min);
	_sigma_pixel_dist_min_v . push_back(sigma_pixel_dist_min);
	_angular_sum_min_v      . push_back(angular_sum_min);
	_triangle_d_min_v       . push_back(triangle_d_min);


	_length_max_v           . push_back(length_max);
	_width_max_v            . push_back(width_max);
	_perimeter_max_v        . push_back(perimeter_max);
	_area_max_v             . push_back(area_max);
	_npixel_max_v           . push_back(npixel_max);
	_qsum_max_v             . push_back(qsum_max);
	_track_frac_max_v       . push_back(track_frac_max);
	_shower_frac_max_v      . push_back(shower_frac_max);
	_mean_pixel_dist_max_v  . push_back(mean_pixel_dist_max);
	_sigma_pixel_dist_max_v . push_back(sigma_pixel_dist_max);
	_angular_sum_max_v      . push_back(angular_sum_max);
	_triangle_d_max_v       . push_back(triangle_d_max);

	_nplanes_v.push_back(nplanes);
	
	  
      } // end particle
      _nparticles = par_id_v.size();

      auto track_iter  = std::max_element(_track_frac_v.begin(),_track_frac_v.end());
      auto shower_iter = std::max_element(_shower_frac_v.begin(),_shower_frac_v.end());

      if (track_iter  == _track_frac_v.end())  throw larbys("Bad Track found");
      if (shower_iter == _shower_frac_v.end()) throw larbys("Bad Shower found");
      
      _track_par_max_id  = track_iter  - _track_frac_v.begin();
      _shower_par_max_id = shower_iter - _shower_frac_v.begin();
      
      _track_par_max_frac  = *track_iter;
      _shower_par_max_frac = *shower_iter;

      std::vector<int> par_type_v(_nparticles,-1.0 * kINVALID_INT);
      std::vector<float> par_frac_v(_nparticles,-1.0 * kINVALID_FLOAT);

      for(size_t parid=0; parid< _nparticles; parid++) {
	auto& par_type = par_type_v[parid];
	auto& par_frac = par_frac_v[parid];

	auto track_frac  = _track_frac_v[parid];
	auto shower_frac = _shower_frac_v[parid];

	data::Info2D info2d;
	
	if (track_frac >= shower_frac) {
	  par_type = 1;
	  par_frac = track_frac;
	  info2d.ptype = data::ParticleType_t::kTrack;
	}
	else {
	  par_type = 2;
	  par_frac = shower_frac;
	  info2d.ptype = data::ParticleType_t::kShower;
	}

	info2d_arr.emplace_back(std::move(info2d));
	AssociateOne(info2d_arr.as_vector().back(),*(par_v[parid]));
      }

      _par1_type = par_type_v.front();
      _par2_type = par_type_v.back();

      _par1_frac = par_frac_v.front();
      _par2_frac = par_frac_v.back();
      
      _tree->Fill();
    } // end this vertex

    _roid += 1;
  }


  void ShapeAnalysis::ClearEvent() {
    _vtxid = kINVALID_INT;
    
    ClearVertex();
  }
  
  void ShapeAnalysis::ClearVertex() {

    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

    _nparticles = kINVALID_INT;

    _nplanes_v.clear();

    _length_v.clear();
    _width_v.clear();
    _perimeter_v.clear();
    _area_v.clear();
    _npixel_v.clear();
    _qsum_v.clear();
    _track_frac_v.clear();
    _shower_frac_v.clear();
    _mean_pixel_dist_v.clear();
    _sigma_pixel_dist_v.clear();
    _angular_sum_v.clear();
    _triangle_d_sum_v.clear();

    _length_max_v.clear();
    _width_max_v.clear();
    _perimeter_max_v.clear();
    _area_max_v.clear();
    _npixel_max_v.clear();
    _qsum_max_v.clear();
    _track_frac_max_v.clear();
    _shower_frac_max_v.clear();
    _mean_pixel_dist_max_v.clear();
    _sigma_pixel_dist_max_v.clear();
    _angular_sum_max_v.clear();
    _triangle_d_max_v.clear();

    _length_min_v.clear();
    _width_min_v.clear();
    _perimeter_min_v.clear();
    _area_min_v.clear();
    _npixel_min_v.clear();
    _qsum_min_v.clear();
    _track_frac_min_v.clear();
    _shower_frac_min_v.clear();
    _mean_pixel_dist_min_v.clear();
    _sigma_pixel_dist_min_v.clear();
    _angular_sum_min_v.clear();
    _triangle_d_min_v.clear();
    
    _track_par_max_id = kINVALID_INT;
    _shower_par_max_id = kINVALID_INT;
    
    _track_par_max_frac = kINVALID_FLOAT;
    _shower_par_max_frac = kINVALID_FLOAT;
    
    _par1_type = kINVALID_INT;
    _par2_type = kINVALID_INT;
    
    _par1_frac = kINVALID_FLOAT;
    _par2_frac = kINVALID_FLOAT;
    
    _vertex_type = kINVALID_INT;
  }

  
}
#endif
