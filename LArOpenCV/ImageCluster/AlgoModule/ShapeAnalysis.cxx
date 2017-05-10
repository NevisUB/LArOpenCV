#ifndef __SHAPEANALYSIS_CXX__
#define __SHAPEANALYSIS_CXX__

#include "ShapeAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelChunk.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

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

    /*
      _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
      _create_compound   = pset.get<bool>("CreateCompound",false);
      _min_particle_size = pset.get<uint>("MinParticleContourSize",3);
      if(_create_compound) {
      _DefectBreaker.set_verbosity(this->logger().level());
      _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
      for(size_t plane=0; plane<3; ++plane)
      Register(new data::TrackClusterCompoundArray);

      _compute_dqdx = pset.get<bool>("CalculatedQdX",false);
      }
    */

    _tree = new TTree("ShapeAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid"       , &_roid      , "roid/I");
    _tree->Branch("vtxid"      , &_vtxid     , "vtxid/I");
    _tree->Branch("nparticles" , &_nparticles, "nparticles/I");

    _tree->Branch("nplanes_v"         , &_nplanes_v);
    _tree->Branch("length_v"          , &_length_v);
    _tree->Branch("width_v"           , &_width_v);
    _tree->Branch("perimeter_v"       , &_perimeter_v);
    _tree->Branch("area_v"            , &_area_v);
    _tree->Branch("npixel_v"          , &_npixel_v);  
    _tree->Branch("track_frac_v"      , &_track_frac_v);
    _tree->Branch("shower_frac_v"     , &_shower_frac_v);
    _tree->Branch("mean_pixel_dist_v" , &_mean_pixel_dist_v);
    _tree->Branch("sigma_pixel_dist_v", &_sigma_pixel_dist_v);
    _tree->Branch("angular_sum_v"     , &_angular_sum_v);

    _roid = 0;
  }

  void ShapeAnalysis::Clear() {
    _nplanes_v.clear();
    _length_v.clear();
    _width_v.clear();
    _perimeter_v.clear();
    _area_v.clear();
    _npixel_v.clear();
    _track_frac_v.clear();
    _shower_frac_v.clear();
    _mean_pixel_dist_v.clear();
    _sigma_pixel_dist_v.clear();
    _angular_sum_v.clear();
  }
  
  void ShapeAnalysis::_Process_() {

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v     = ImageArray(ImageSetID_t::kImageSetWire);
    auto track_img_v   = ImageArray(ImageSetID_t::kImageSetTrack);
    auto shower_img_v  = ImageArray(ImageSetID_t::kImageSetShower);

    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();
      
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      const auto& vtx3d = vtx3d_v[vtxid];
      
      _vtxid = vtxid;
      
      Clear();
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;
      
      for(auto par_id : par_id_v) {
	const auto& par = particle_v[par_id];
	
	float length    = 0.0;
	float width     = 0.0;
	float perimeter = 0.0;
	float area  = 0.0;
	float npixel = 0.0;
	float track_frac = 0.0;
	float shower_frac = 0.0;
	float mean_pixel_dist = 0.0;
	float sigma_pixel_dist = 0.0;
	float angular_sum = 0.0;

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
	  track_frac       += pchunk.track_frac;
	  shower_frac      += pchunk.shower_frac;
	  mean_pixel_dist  += pchunk.mean_pixel_dist;
	  sigma_pixel_dist += pchunk.sigma_pixel_dist;
	  angular_sum      += pchunk.angular_sum;
	}

	_length_v.push_back(length);
	_width_v.push_back(width);
	_perimeter_v.push_back(perimeter);
	_area_v.push_back(area);
	_npixel_v.push_back(npixel);
	_track_frac_v.push_back(track_frac);
	_shower_frac_v.push_back(shower_frac);
	_mean_pixel_dist_v.push_back(mean_pixel_dist);
	_sigma_pixel_dist_v.push_back(sigma_pixel_dist);
	_angular_sum_v.push_back(angular_sum);
	_nplanes_v.push_back(nplanes);
      }
      _nparticles = par_id_v.size();

      _tree->Fill();
    } // end this vertex

    _roid += 1;

    if(NextEvent()) _roid=0;
  }

  /*
    if(_create_compound) {
    auto& particle = par_data.as_vector().back();
    // Create cluster compound, order the atomics, determine atomic edges
    auto cluscomp = _DefectBreaker.BreakContour(particle._ctor);
    const auto ordered_atom_id_v = _AtomicAnalysis.OrderAtoms(cluscomp,circle_vtx.center);
    auto atom_edges_v = _AtomicAnalysis.AtomsEdge(cluscomp, circle_vtx.center, ordered_atom_id_v);

    for(auto atom_id : ordered_atom_id_v) {
    auto& atom = cluscomp.get_atomic(atom_id);
    auto& start_end = atom_edges_v[atom_id];
    atom.set_start(start_end.first);
    atom.set_end(start_end.second);
    }
    // set the atomic order, get the farthest edge, set the end point
    cluscomp.set_atomic_order(ordered_atom_id_v);
    auto& last_start_end = atom_edges_v.at(ordered_atom_id_v.back());
    cluscomp.set_end_pt(last_start_end.second);
	    
    if(_compute_dqdx) {
    for (auto& atomic : cluscomp){
    auto pca = CalcPCA(atomic);
    auto dqdx = _AtomicAnalysis.AtomdQdX(img, atomic, pca, atomic.start_pt(), atomic.end_pt());
    atomic.set_dqdx(dqdx);
    atomic.set_pca(pca);
    LAROCV_DEBUG() << "... calculated dqdx " << dqdx.size() << std::endl;
    }
    }
	    
    // Store, ans associate TrackClusterCompound to ParticleCluster, and to Vertex3D
    auto& compound_v = AlgoData<data::TrackClusterCompoundArray>(3+plane);
    compound_v.emplace_back(std::move(cluscomp));
    AssociateMany(vtx3d,compound_v.as_vector().back());
    AssociateOne(particle,compound_v.as_vector().back());
    }
  */

  bool ShapeAnalysis::_PostProcess_() const {

    return true;
  }
}
#endif
