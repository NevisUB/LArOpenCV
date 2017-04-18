#ifndef __TRACKVERTEXSCAN_CXX__
#define __TRACKVERTEXSCAN_CXX__

#include "TrackVertexScan.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"

namespace larocv {

  /// Global larocv::TrackVertexScanFactory to register AlgoFactory
  static TrackVertexScanFactory __global_TrackVertexScanFactory__;
  
  void TrackVertexScan::_Configure_(const Config_t &pset)
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

    // 2D Vertex Points
    auto const vtx_algo_name = pset.get<std::string>("VertexEstimateAlgo");
    _vertex_estimate_algo_id = this->ID(vtx_algo_name);
    

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

  bool TrackVertexScan::_PostProcess_() const
  { return true; }

  void TrackVertexScan::_Process_() {

    //
    // 0) Prep vertex to be scanned from vertex estimate
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
	
    //std::cout<<"the algorithm id is "<< _vertex_estimate_algo_id<<std::endl;
    const auto& cand_vtx_v = AlgoData<data::Vertex3DArray>(_vertex_estimate_algo_id,0).as_vector();
    
    //
    // 1) Scan for a 3D vertex @ candidate seeds
    //
    std::vector<data::VertexSeed3D> vertex3dseed_v;
    
    for(auto& cand_vtx3d : cand_vtx_v) {

      // Scan 3D region centered @ this vertex seed
      auto cand_vtx = data::Seed2Vertex(cand_vtx3d);
      auto vtx3d    = _VertexScan3D.RegionScan3D(cand_vtx, img_v);
      
      size_t num_good_plane = 0;
      double dtheta_sum = 0;

      int plane = -1;
      // Require atleast 2 crossing point on 2 planes (using ADC image)
      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
	plane=+1;
	LAROCV_DEBUG() << "Found " << cvtx2d.xs_v.size() << " xs on plane " << plane << std::endl;
	//std::cout<< "center x is "<< cvtx2d.center.x<<"center y is"<<cvtx2d.center.y<<std::endl;
	//std::cout<< "radius is "<< cvtx2d.radius<<std::endl;
	for(auto const & cvtx : cvtx2d.xs_v){
	  //std::cout<< "xs x is "<< cvtx.pt.x << "xs y is "<< cvtx.pt.y<<std::endl;
	}
	if(cvtx2d.xs_v.size()<2) continue;
	LAROCV_DEBUG() << "... accepted" << std::endl;
        num_good_plane++;
        dtheta_sum += cvtx2d.sum_dtheta();
      }
      
      if(num_good_plane<2) {
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
    } // end candidate vertex seed


    //
    // 2) Merging prodecure for verticies within given 3D range
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
