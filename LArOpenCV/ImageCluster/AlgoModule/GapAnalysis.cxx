#ifndef __GAPANALYSIS_CXX__
#define __GAPANALYSIS_CXX__

#include "GapAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::GapAnalysisFactory to register AlgoFactory
  static GapAnalysisFactory __global_GapAnalysisFactory__;
  
  void GapAnalysis::_Configure_(const Config_t &pset)
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
    
    
    _tree = new TTree("GapAnalysis","");

    AttachIDs(_tree);
    _tree->Branch("roid" , &_roid  , "roid/I");
    _tree->Branch("vtxid", &_vtxid , "vtxid/I");
    _tree->Branch("x"    , &_x     , "x/D");
    _tree->Branch("y"    , &_y     , "y/D");
    _tree->Branch("z"    , &_z     , "z/D");
        
    _tree->Branch("pathexists1"   , &_pathexists1  ,"pathexists1/I");
    _tree->Branch("pathexists2"   , &_pathexists2  ,"pathexists2/I");

    _tree->Branch("pathexists1_v", &_pathexists1_v);
    _tree->Branch("pathexists2_v", &_pathexists2_v);
    
    _tree->Branch("infiducial"    , &_infiducial, "infiducial/I");

    _roid  = 0;
  }

  void GapAnalysis::_Process_() {
    LAROCV_INFO() << "start" << std::endl;
    ClearEvent();
    
    if(NextEvent()) _roid=0;
    
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto adc_thresh_img_v = adc_img_v;
    for(auto& v : adc_thresh_img_v) v = larocv::Threshold(v,10,255);

    auto& ass_man = AssManager();

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
      
      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      _vtxid += 1;

      auto nparticles = par_id_v.size();

      //
      // Particle wise check if path exists (2 different ways)
      //
      std::vector<std::vector<int> > plane_path_exists_vv;
      std::vector<std::vector<int> > circle_path_exists_vv;
      plane_path_exists_vv.resize(nparticles);
      circle_path_exists_vv.resize(nparticles);
	
      const auto& cvtx2d_v = vtx3d.cvtx2d_v;
      
      for(size_t par_idx = 0; par_idx < par_id_v.size(); ++par_idx) {
	auto par_id = par_id_v[par_idx];

	const auto& par = particle_v[par_id];
		
	auto& plane_path_exists_v  = plane_path_exists_vv[par_idx];
	auto& circle_path_exists_v = circle_path_exists_vv[par_idx];
	
	plane_path_exists_v.resize(3,kINVALID_INT);
	circle_path_exists_v.resize(3,kINVALID_INT);
	
	for(size_t plane=0; plane<3; ++plane) {

	  const auto& pcluster = par._par_v[plane];
	  const auto& pctor = pcluster._ctor;
	  if (pctor.empty()) continue;

	  auto& plane_path_exists = plane_path_exists_v[plane];
	  auto& circle_path_exists= circle_path_exists_v[plane];

	  const auto& cvtx = cvtx2d_v.at(plane);

	  geo2d::VectorArray<float> xs_v;
	  xs_v.reserve(cvtx.xs_v.size());
	  for(auto v : cvtx.xs_v) xs_v.emplace_back(v.pt);
	  
	  auto circle = cvtx.as_circle();
	  if (circle.radius <= 0) throw larbys("circle rad sz < 0");
	  const auto& thresh_img = adc_thresh_img_v.at(plane);
	  auto mask_img = larocv::MaskImage(thresh_img,circle,0,false);
	  auto mask_ctor_v = larocv::FindContours(mask_img);
	  
	  // Check if the crossing points are withen the same contour
	  std::vector<size_t> parent_ctor_id_v(xs_v.size(),kINVALID_SIZE);
	  for(size_t xs_id=0; xs_id < xs_v.size(); ++xs_id) {
	    parent_ctor_id_v.at(xs_id) = larocv::FindContainingContour(mask_ctor_v,xs_v.at(xs_id));
	  }
	  
	  bool different = false;

	  // They do not have the same parent id, they are different
	  auto this_ctor_id = parent_ctor_id_v.front();
	  for(auto parent_ctor_id : parent_ctor_id_v) {
	    if (parent_ctor_id == kINVALID_SIZE) continue;
	    if (parent_ctor_id != this_ctor_id) 
	      { different = true; break; }
	  }
	  
	  // There is only one crossing point, they are different
	  if (xs_v.size()==1) different = true;

	  // They are different? Then @ this plane there is gap
	  if (different) plane_path_exists = 0;

	  // They are attached
	  else plane_path_exists = 1;

	  double min_dist = kINVALID_DOUBLE;
	  geo2d::Vector<float> min_pt;
	  
	  for(const auto& xs : xs_v) {
	    double dist = kINVALID_DOUBLE;
	    auto inside = larocv::PointPolygonTest(pctor,xs,dist);
	    if (inside) { min_dist = 0; min_pt = xs; break; }
	    if (dist < min_dist) { min_dist = dist; min_pt = xs; }
	  }
	  // Check if the vertex is in the same contour as the particle
	  if (xs_v.size()==1) circle_path_exists = 0;
	  else  circle_path_exists = larocv::PathExists(mask_img,circle.center,min_pt,5,0,1);
	  
	} // end plane
      } // end particle


      _pathexists1_v.clear();
      _pathexists2_v.clear();
      _pathexists1_v.resize(nparticles,1);
      _pathexists2_v.resize(nparticles,1);
      
      for(size_t pidx=0; pidx < nparticles; ++pidx) {
	size_t valid_plane_path_exists = 0;
	size_t valid_circle_path_exists = 0;

	//
	// Gap check 1
	//
	int plane_path_exists = 0;
	for(auto path_plane : plane_path_exists_vv[pidx]) {
	  if (path_plane == kINVALID_INT) continue;
	  valid_plane_path_exists += 1;
	  plane_path_exists += path_plane;
	}

	if (valid_plane_path_exists == 2 && plane_path_exists == 0 ) 
	  _pathexists1_v[pidx] = 0;
	
	if (valid_plane_path_exists == 3 && plane_path_exists <= 1 )
	  _pathexists1_v[pidx] = 0;

	//
	// Gap check 2
	//
	int circle_path_exists = 0;
	for(auto path_circle : circle_path_exists_vv[pidx]) {
	  if (path_circle == kINVALID_INT) continue;
	  valid_circle_path_exists += 1;
	  circle_path_exists  += path_circle;
	}

	if (valid_circle_path_exists == 2 && circle_path_exists == 0 ) 
	  _pathexists2_v[pidx] = 0;
	
	if (valid_circle_path_exists == 3 && circle_path_exists <= 1 )
	  _pathexists2_v[pidx] = 0;
      }

      _pathexists1 = 1;
      _pathexists2 = 1;
	
      for(auto pe : _pathexists1_v) {
	if (!pe) { _pathexists1 = 0; }
      }

      for(auto pe : _pathexists2_v) {
	if (!pe) { _pathexists2 = 0; }
      }
      
      _infiducial=1;
      if( _x < 5.     || _x > 251.35 ||
	  _y < -111.5 || _y > 111.5  ||
	  _z < 5.     || _z > 1031.8 )
	_infiducial = 0;

      _tree->Fill();
    } // end vertex

    _roid += 1;
    LAROCV_INFO() << "end" << std::endl;
  }

  void GapAnalysis::ClearEvent() {
    _vtxid = kINVALID_INT;
    ClearVertex();
  }
  

  void GapAnalysis::ClearVertex() {

    _x = kINVALID_DOUBLE;   
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;
    
    _pathexists1 = kINVALID_INT;
    _pathexists2 = kINVALID_INT;
    
    _pathexists1_v.clear();
    _pathexists2_v.clear();

    _infiducial = kINVALID_INT;
  }
  
}
#endif
