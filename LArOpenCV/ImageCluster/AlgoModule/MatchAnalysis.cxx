#ifndef __MATCHANALYSIS_CXX__
#define __MATCHANALYSIS_CXX__

#include "MatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

#include <array>

namespace larocv {
  
  static MatchAnalysisFactory __global_MatchAnalysisFactory__;
  
  void MatchAnalysis::_Configure_(const Config_t &pset)
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

    _break_contours = pset.get<bool>("BreakContours");

    if(_break_contours)
      _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
    
    _AtomicAnalysis.Configure(pset.get<Config_t>("AtomicAnalysis"));
    _VertexAnalysis.Configure(pset.get<Config_t>("VertexAnalysis"));
    
    _tree = new TTree("MatchAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"       , &_roid      , "roid/I");
    _tree->Branch("vtxid"      , &_vtxid     , "vtxid/I");
    _tree->Branch("x"          , &_x         , "x/D");
    _tree->Branch("y"          , &_y         , "y/D");
    _tree->Branch("z"          , &_z         , "z/D");

    _tree->Branch("par_pixel_ratio_v",&_par_pixel_ratio_v);
    _tree->Branch("par_valid_end_pt_v",&_par_valid_end_pt_v);
    _tree->Branch("par_end_pt_x_v",&_par_end_pt_x_v);
    _tree->Branch("par_end_pt_y_v",&_par_end_pt_y_v);
    _tree->Branch("par_end_pt_z_v",&_par_end_pt_z_v);
    _tree->Branch("par_n_planes_charge_v",&_par_n_planes_charge_v);
    _tree->Branch("par_3d_PCA_theta_estimate_v",&_par_3d_PCA_theta_estimate_v);
    _tree->Branch("par_3d_PCA_phi_estimate_v",&_par_3d_PCA_phi_estimate_v);
    _tree->Branch("par_3d_segment_theta_estimate_v",&_par_3d_segment_theta_estimate_v);
    _tree->Branch("par_3d_segment_phi_estimate_v",&_par_3d_segment_phi_estimate_v);
    _tree->Branch("vertex_n_planes_charge",_vertex_n_planes_charge,"vertex_n_planes_charge/I");
  
    _roid = 0;
  }
  
  void MatchAnalysis::Clear() {
    _par_pixel_ratio_v.clear();
    _par_valid_end_pt_v.clear();
    _par_end_pt_x_v.clear();
    _par_end_pt_y_v.clear();
    _par_end_pt_z_v.clear();
    _par_n_planes_charge_v.clear();
    _par_3d_PCA_theta_estimate_v.clear();
    _par_3d_PCA_phi_estimate_v.clear();
    _par_3d_segment_theta_estimate_v.clear();
    _par_3d_segment_phi_estimate_v.clear();
    _vertex_n_planes_charge = kINVALID_INT;
  }

  void MatchAnalysis::_Process_() {
    
    if(NextEvent()) _roid=0;
    
    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);

    const auto& meta_v = MetaArray();
    for(const auto& meta : meta_v)
      _VertexAnalysis.ResetPlaneInfo(meta);
    
    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();

    _vtxid = -1;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;

      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;

      Clear();

      size_t npar = par_id_v.size();
      
      _par_pixel_ratio_v.resize(npar);
      _par_valid_end_pt_v.resize(npar);
      _par_end_pt_x_v.resize(npar);
      _par_end_pt_y_v.resize(npar);
      _par_end_pt_z_v.resize(npar);
      _par_n_planes_charge_v.resize(npar);
      _par_3d_PCA_theta_estimate_v.resize(npar);
      _par_3d_PCA_phi_estimate_v.resize(npar);
      _par_3d_segment_theta_estimate_v.resize(npar);
      _par_3d_segment_phi_estimate_v.resize(npar);
     
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];

	//
	// do something with this particle
	//
	auto& par_pixel_ratio = _par_pixel_ratio_v[par_id];
	auto& par_valid_end_pt = _par_valid_end_pt_v[par_id];
	auto& par_end_pt_x = _par_end_pt_x_v[par_id];
	auto& par_end_pt_y = _par_end_pt_y_v[par_id];
	auto& par_end_pt_z = _par_end_pt_z_v[par_id];
	auto& par_n_planes_charge = _par_n_planes_charge_v[par_id];
	auto& par_3d_PCA_theta_estimate = _par_3d_PCA_theta_estimate_v[par_id];
	auto& par_3d_PCA_phi_estimate = _par_3d_PCA_phi_estimate_v[par_id];
	auto& par_3d_segment_theta_estimate = _par_3d_segment_theta_estimate_v[par_id];
	auto& par_3d_segment_phi_estimate = _par_3d_segment_phi_estimate_v[par_id];

	//
	// compute the number of planes this particle is on
	//
	par_n_planes_charge = 0;
	
	//
	// compute the pixel ratio
	// for 2 particles ratio small/large
	// for (small/large + middle/large) / 2
	//
	std::array<double,3> pixel_arr;
	for(auto& v : pixel_arr) v=kINVALID_DOUBLE;
	
	//
	// determine the end point of track clusters
	//
	std::array<data::TrackClusterCompound,3> cluscomp_arr;
	std::array<const data::TrackClusterCompound*,3> cluscomp_ptr_arr;
	for(auto& v : cluscomp_ptr_arr) v=nullptr;

	//
	// particle cluster loop
	//
	for(size_t plane=0; plane<3; ++plane) {

	  const auto& pcluster = par._par_v[plane];
	  const auto& ctor = pcluster._ctor;

	  if(ctor.empty()) continue;
	  
	  par_n_planes_charge += 1;
	  
	  //
	  // compute the pixel ratio
	  //
	  pixel_arr[plane] = CountNonZero(adc_img_v.at(plane),ctor,0);
	  
	  //
	  // determine the end point of track clusters
	  //
	  auto& cluscomp = cluscomp_arr[plane]; 
	  cluscomp_ptr_arr[plane] = &cluscomp_arr[plane];
	  
	  if (_break_contours) {
	    cluscomp = _DefectBreaker.BreakContour(ctor);
	  } else {
	    data::AtomicContour atomic_ctor(ctor,0);
	    cluscomp.emplace_back(std::move(atomic_ctor));
	  }
	  
	  LAROCV_DEBUG() << "Broke 1 contour sz " << ctor.size()
			 << " into " << cluscomp.size()
			 << " atomics" << std::endl;

	  auto vtx2d = vtx3d.cvtx2d_v.at(plane);
	    
	  auto ordered_atom_id_v = _AtomicAnalysis.OrderAtoms(cluscomp,vtx2d.center);
	  cluscomp.set_atomic_order(ordered_atom_id_v);
	  
	  auto atom_edges_v = _AtomicAnalysis.AtomsEdge(cluscomp, vtx2d.center, ordered_atom_id_v);
	  
	  for (size_t atom_id=0; atom_id<cluscomp.size(); ++atom_id) {
 	    auto& atomic = cluscomp[atom_id];
	    auto& start_end = atom_edges_v[atom_id];
	    atomic.add_edge(start_end.first);
	    atomic.add_edge(start_end.second);
	    if (atomic.id() == ordered_atom_id_v.back()) {
	      _AtomicAnalysis.RefineAtomicEndPoint(adc_img_v[plane],atomic);
	      cluscomp.set_end_pt(atomic.edges()[1]);
	    }
	  } // end this atomic
	} // end this plane cluster

	//
	// compute the pixel ratio 
	//
	std::sort(pixel_arr.begin(),pixel_arr.end());

	auto largest = pixel_arr.back();

	if (largest == kINVALID_DOUBLE) 
	  par_pixel_ratio = pixel_arr[0] / pixel_arr[1];
	else
	  par_pixel_ratio = (pixel_arr[0] + pixel_arr[1]) / (2 * largest);

	//
	// determine the end point of track clusters
	//
	data::Vertex3D end3d;
	auto edge_found = _VertexAnalysis.MatchEdge(cluscomp_ptr_arr,end3d);
	par_valid_end_pt = edge_found;
	
	if(edge_found) {
	  par_end_pt_x = end3d.x;
	  par_end_pt_y = end3d.y;
	  par_end_pt_z = end3d.z;
	}
	
      } // end this particle
      
      //
      // determine n planes charge @ vertex
      //
      _vertex_n_planes_charge = 0;
      for(size_t plane=0; plane<3; ++plane) {
	const auto& pt2d = vtx3d.vtx2d_v[plane];
	geo2d::Circle<float> circle(pt2d.pt,6);
	auto npx = CountNonZero(MaskImage(thresh_img_v[plane],circle,0,false));
	if(npx) _vertex_n_planes_charge += 1;
      }
      
      
      _tree->Fill();
    } // end this vertex

    _roid += 1;
  }

  // std::pair<float,float> MatchAnalysis::Angle3D(data::Vertex& vtx1,
  // 						data::Vertex& vtx2) {

  //   auto res_dist = Distance(vtx1,vtx2);
  //   auto res_vtx  = Difference(vtx1,vtx2);
    
  //   if (res_dist == 0) throw larbys("Vertex1 and Vertex2 cannot be the same");
    
  //   auto cos = res_vertex.z / res_dist;
  //   auto tan = res_vertex.y / res_vertex.x;

  //   auto arccos = std::acos(cos);
  //   auto atan   = std::atan2(tan);
    
  //   return std::make_pair(arccos, atan);
  // }  
  
  // std::pair<float,float> MatchAnalysis::Angle3D(const data::Particle& particle,
  // 						const std::vector<cv::Mat>& img_v) {
    
  //   // accumulate some 3D points across matched ParticleClusters
  //   std::vector<data::Vertex3D> vertex_v;

  //   // get the enclosed
  //   GEO2D_ContourArray_t ctor_v;
    
    
    
  //   return ;
  // }
  
}
#endif
