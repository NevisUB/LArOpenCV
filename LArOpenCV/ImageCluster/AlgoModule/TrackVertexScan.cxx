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
    
    const auto& cand_vtx_v = AlgoData<data::Vertex3DArray>(_vertex_estimate_algo_id,0).as_vector();
    
    std::vector<cv::Mat> img_thresh_v;
    img_thresh_v.reserve(3);
    for(auto& im : img_v)
      img_thresh_v.emplace_back(larocv::Threshold(im,10,255));
    
    //
    // 1) Scan for a 3D vertex @ registered candidate seeds
    //
    _VertexScan3D.RegisterRegions(cand_vtx_v);
    auto vertex3d_v = _VertexScan3D.RegionScan3D(img_thresh_v);
    
    auto& vertex3darr = AlgoData<data::Vertex3DArray>(0);
    for(auto& vertex3d : vertex3d_v)
      vertex3darr.emplace_back(std::move(vertex3d));
    
  }
}
#endif
