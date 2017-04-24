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
      _allowed_radius=pset.get<float>("SearchRadiusSize"); //5
    
    Register(new data::Vertex3DArray);
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
	
    //std::cout<<"the algorithm id is "<< _vertex_estimate_algo_id<<std::endl;
    const auto& cand_vtx_v = AlgoData<data::Vertex3DArray>(_vertex_estimate_algo_id,0).as_vector();
    
    //
    // 1) Scan for a 3D vertex @ candidate seeds
    //
    std::vector<data::Vertex3D> vertex3d_v;

    std::vector<cv::Mat> img_thresh_v;
    img_thresh_v.reserve(3);
    for(auto& im : img_v)
      img_thresh_v.emplace_back(larocv::Threshold(im,10,255));
    
    LAROCV_DEBUG() << "See " << cand_vtx_v.size() << " 3D candidate track vertices" << std::endl;
    //for(auto& cand_vtx3d : cand_vtx_v) {
    for(size_t vertex_id=0; vertex_id<cand_vtx_v.size();++vertex_id) {
      const auto& cand_vtx3d = cand_vtx_v[vertex_id];
      LAROCV_DEBUG() << "Vertex " << vertex_id << std::endl;
      LAROCV_DEBUG() << "OLD estimate @ (x,y,z)=("<<cand_vtx3d.x<<","<<cand_vtx3d.y<<","<<cand_vtx3d.z<<")"<<std::endl;

      for(size_t plane=0;plane<3;++plane) {
	const auto& vtx2d = cand_vtx3d.vtx2d_v.at(plane);
	LAROCV_DEBUG() << plane << ") (x,y)=(" << vtx2d.pt.x << "," << vtx2d.pt.y << ")" << std::endl;
      }
	
      // Scan 3D region centered @ this vertex seed
      auto vtx3d  = _VertexScan3D.RegionScan3D(data::VertexSeed3D(cand_vtx3d), img_thresh_v);
      
      size_t num_good_plane = 0;
      double dtheta_sum = 0;

      int plane = -1;
      // Require atleast 2 crossing point on 2 planes (using ADC image)
      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
	plane += 1;
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

      // Optional: require there to be some charge in vincinity of projected vertex on 3 planes
      uint nvalid=0;
      if(_require_3planes_charge) {
	LAROCV_DEBUG() << "Requiring 3 planes charge in circle... " << std::endl;
	for(size_t plane=0;plane<3;++plane)  {
	  auto vtx2d= vtx3d.vtx2d_v[plane];
	  auto npx = CountNonZero(img_thresh_v[plane],geo2d::Circle<float>(vtx2d.pt,_allowed_radius));
	  LAROCV_DEBUG() << "@ (" << vtx2d.pt.x << "," << vtx2d.pt.y
			 << ") w/ rad " << _allowed_radius
			 << " see " << npx << " nonzero pixels" << std::endl;
	  if(npx) nvalid++;
	}
	if(nvalid!=3) {
	  LAROCV_DEBUG() << "... invalid, SKIP!" << std::endl;
	  continue;
	}
      }
      
      // Move seed into the output
      vertex3d_v.emplace_back(std::move(vtx3d));
      LAROCV_DEBUG() << "AlgoData size @ " << vertex3d_v.size() << std::endl;
    } // end candidate vertex seed
    
    auto& vertex3darr = AlgoData<data::Vertex3DArray>(0);
    for(auto& vertex3d : vertex3d_v)
      vertex3darr.emplace_back(std::move(vertex3d));
   


    
  }
}
#endif
