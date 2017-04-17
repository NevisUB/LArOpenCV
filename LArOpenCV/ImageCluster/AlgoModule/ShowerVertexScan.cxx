#ifndef __SHOWERVERTEXSCAN_CXX__
#define __SHOWERVERTEXSCAN_CXX__

#include "ShowerVertexScan.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"

namespace larocv {

  /// Global larocv::ShowerVertexScanFactory to register AlgoFactory
  static ShowerVertexScanFactory __global_ShowerVertexScanFactory__;
  
  void ShowerVertexScan::_Configure_(const Config_t &pset)
  {
    //
    // Prepare algorithms via config
    //

    // LArPlaneGeo, and image space to real space helping class
    _geo.set_verbosity(this->logger().level());
    _geo.Configure(pset.get<Config_t>("LArPlaneGeo"));

    // VertexScan3D -- 3D search for candidate vertices
    _VertexScan3D.set_verbosity(this->logger().level());
    _VertexScan3D.Configure(pset.get<Config_t>("VertexScan3D"));

    // 2D edge producer module
    auto name_seed = pset.get<std::string>("EdgeSeedProducer");
    _seed_id=kINVALID_ALGO_ID;
    if (!name_seed.empty()) {
      _seed_id = this->ID(name_seed);
      if (_seed_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    // Merge very nearby candidate vertices
    _merge_nearby = pset.get<bool>("MergeNearby",true);
    if(_merge_nearby) 
      _merge_distance = pset.get<float>("MergeDistance",3);

    // Optionally require 3 planes charge in vicinity of projected vertex location
    _require_3planes_charge=pset.get<bool>("Require3PlanesCharge");
    
    if(_require_3planes_charge)
      _allowed_radius=pset.get<float>("SearchRadiusSize");//5      

    Register(new data::VertexSeed3DArray);
  }

  bool ShowerVertexScan::_PostProcess_() const
  { return true; }

  void ShowerVertexScan::_Process_() {

    //
    // 0) Prep
    //
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();

    // Inform algorithms of image information (will change per ROI processed)
    for(auto const& meta : meta_v) {
      _geo.ResetPlaneInfo(meta);
      _vtxana.ResetPlaneInfo(meta);
      _VertexScan3D.SetPlaneInfo(meta);
    }
    
    auto nplanes = img_v.size();

    std::vector<std::vector<const data::VertexSeed2D*> > seed_vv;
    seed_vv.resize(nplanes);
		   
    for(size_t plane=0;plane<nplanes;++plane) {

      // Access AlgoData vertex seeds from previous algorithm
      const auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(_seed_id,plane);
      const auto& vertexseed2d_v = vertexseed2darray.as_vector();
      
      auto& seed_v = seed_vv[plane];
      seed_v.reserve(vertexseed2d_v.size());

      // Insert a pointer to this seed into vector
      for(const auto& vertexseed2d : vertexseed2d_v) {
	seed_v.push_back(&vertexseed2d);
      }
      LAROCV_DEBUG() << "Got " << seed_v.size() << " seed @ plane " << plane << std::endl;
    }
    
    //
    // 1) Compute pairwire 3D verticies with a generous time allowance
    //
    
    std::vector<data::Vertex3D> cand_vtx_v;
    
    for(size_t plane0=0;plane0<nplanes;++plane0) {
      const auto& seed0_v = seed_vv[plane0];
      for(size_t plane1=plane0+1;plane1<nplanes;++plane1) {
	const auto& seed1_v = seed_vv[plane1];

	for(size_t seed0_id=0;seed0_id<seed0_v.size();++seed0_id) {
	  const auto seed0 = seed0_v[seed0_id];
	  for(size_t seed1_id=0;seed1_id<seed1_v.size();++seed1_id) {
	    const auto seed1 = seed1_v[seed1_id];
	    
	    data::Vertex3D vtx;
	    auto res = _geo.YZPoint(*seed0,plane0,*seed1,plane1,vtx);
	    // could vertex be made?
	    if(!res) continue;

	    cand_vtx_v.emplace_back(std::move(vtx));
	    
	    LAROCV_DEBUG()<<"Plane ("<<plane0<<","<<plane1<<") seeds ("<<seed0_id<<","<<seed1_id<<") @ ["<<*seed0<<","<<*seed1<<"] " << res << std::endl;
	    for(size_t plane=0;plane<nplanes;++plane) {
	      LAROCV_DEBUG()<<"... "<< plane<<") @ "<<vtx.vtx2d_v[plane].pt<<std::endl;
	    }
	    LAROCV_DEBUG()<<"... dseed0 "<<geo2d::dist(*seed0,vtx.vtx2d_v[plane0].pt)<<" dseed1 "<<geo2d::dist(*seed1,vtx.vtx2d_v[plane1].pt)<<std::endl;

	  } //seed1
	} //seed0
      } //plane1
    } //plane0

    //
    // 2) Scan for a 3D vertex @ candidate seeds
    //
    std::vector<data::VertexSeed3D> vertex3dseed_v;
    
    for(auto& cand_vtx3d : cand_vtx_v) {

      // For this 3D candidate seed, fill the 2D projection (VertexSeed3D::vtx2d_v)
      try {
	_vtxana.UpdatePlanePosition(cand_vtx3d,_geo);
      }
      catch(const larbys& err) {
	LAROCV_WARNING() << "Predicted vertex has 2D point outside image" << std::endl;
	// Skip this candidate if the projected point is outside the image
	continue;
      }

      // Scan 3D region centered @ this vertex seed
      auto cand_vtx = data::Seed2Vertex(cand_vtx3d);
      auto vtx3d    = _VertexScan3D.RegionScan3D(cand_vtx, img_v);
      
      size_t num_good_plane = 0;
      double dtheta_sum = 0;

      int plane = -1;
      // Require atleast 2 crossing point on 2 planes (using ADC image)
      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
	plane = +1;
	LAROCV_DEBUG() << "Found " << cvtx2d.xs_v.size() << " xs on plane " << plane << std::endl;
        if(cvtx2d.xs_v.size()<2) continue;
	LAROCV_DEBUG() << "... accepted" << std::endl;
        num_good_plane++;
        dtheta_sum += cvtx2d.sum_dtheta();
      }
      
      if(num_good_plane < 2) {
	LAROCV_DEBUG() << "Num good plane < 2, SKIP!!" << std::endl;
	continue;
      }
      dtheta_sum /= (double)num_good_plane;
      LAROCV_DEBUG() << "Registering vertex seed type="<<(uint)cand_vtx3d.type
		     << " @ ("<<vtx3d.x<<","<<vtx3d.y<<","<<vtx3d.z<<")"<<std::endl;

      // Create a edge type vertex seed
      data::VertexSeed3D seed(vtx3d);
      seed.type = data::SeedType_t::kEdge;

      // Optional: require there to be some charge in vincinity of projected vertex on 3 planes
      uint nvalid=0;
      if(_require_3planes_charge) {
	LAROCV_DEBUG() << "Requiring 3 planes charge in circle... " << std::endl;
	for(size_t plane=0;plane<3;++plane)  {
	  auto pt= seed.vtx2d_v[plane];
	  LAROCV_DEBUG() << "@ " << pt << " rad " << _allowed_radius << std::endl;
	  auto npx = CountNonZero(img_v[plane],geo2d::Circle<float>(pt,_allowed_radius));
	  LAROCV_DEBUG() << plane << ") @ " << seed.vtx2d_v[plane] << " see " << npx << " inside" << std::endl;
	  if(npx) nvalid++;
	}
	if(nvalid!=3) {
	  LAROCV_DEBUG() << "... invalid, SKIP!!" << std::endl;
	  continue;
	}
      }
      
      // Move seed into the output
      vertex3dseed_v.emplace_back(std::move(seed));
      LAROCV_DEBUG() << "AlgoData size @ " << vertex3dseed_v.size() << std::endl;
    } // end candidate vertex seed


    //
    // 3) Merging prodecure for verticies within given 3D range
    //

    /*
    LAROCV_DEBUG() << "Merging nearby start @ " << cand_vtx_v.size() << std::endl;
    if(_merge_nearby) _vtxana.MergeNearby(cand_vtx_v,_merge_distance);
    LAROCV_DEBUG() << "& end with " << cand_vtx_v.size() << std::endl;
    */

    auto& vertex3dseedarr = AlgoData<data::VertexSeed3DArray>(0);
    for(auto& vertex3dseed : vertex3dseed_v)
      vertex3dseedarr.emplace_back(std::move(vertex3dseed));
    
  }
}
#endif
