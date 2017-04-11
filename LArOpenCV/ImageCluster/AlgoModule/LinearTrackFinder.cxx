#ifndef __LINEARTRACKFINDER_CXX__
#define __LINEARTRACKFINDER_CXX__

#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Geo2D.h"
#include "LinearTrackFinder.h"
#include "Geo2D/Core/spoon.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrack.h"
#include <map>
#include <array>

namespace larocv {

  /// Global larocv::LinearTrackFinderFactory to register AlgoFactory
  static LinearTrackFinderFactory __global_LinearTrackFinderFactory__;

  void LinearTrackFinder::_Configure_(const Config_t &pset)
  {
    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("SingleLinearTrack"));
    
    auto algo_name_part = pset.get<std::string>("TrackVertexEstimate","");

    _algo_id_part = kINVALID_ALGO_ID;

    if(!algo_name_part.empty())
      _algo_id_part = this->ID( algo_name_part );
    
    Register(new data::LinearTrack3DArray);
  }
  
  void LinearTrackFinder::_Process_(const larocv::Cluster2DArray_t& clusters,
				    ::cv::Mat& img,
				    larocv::ImageMeta& meta,
				    larocv::ROI& roi)
  { _algo.SetPlaneInfo(meta); }

  bool LinearTrackFinder::_PostProcess_(std::vector<cv::Mat>& img_v)
  {

    if (_algo_id_part != kINVALID_ALGO_ID) {
      // Get a list of vertx already found in the image
      auto const& vtx3d_v = AlgoData<data::Vertex3DArray> ( _algo_id_part, 0 );
      std::vector<std::vector<geo2d::Vector<float> > > vtx2d_vv;
      for(auto const& vtx3d : vtx3d_v.as_vector()) {
	for(size_t plane=0; plane<vtx3d.num_planes; ++plane) {
	  vtx2d_vv.resize(plane+1);
	  vtx2d_vv[plane].push_back(vtx3d.cvtx2d_v.at(plane).center);
	}
      }

      // Register to SingleLinearTrack algorithm to search for an independent (separated) single track
      for(size_t plane=0; plane<vtx2d_vv.size(); ++plane)
	
	_algo.RegisterVertex2D(plane,vtx2d_vv[plane]);
    }
    
    // Find linear track
    auto ltrack_v = _algo.FindLinearTrack(img_v);    
    LAROCV_DEBUG() << "Found linear track size " << ltrack_v.size() << std::endl;

    // Record
    auto& data = AlgoData<larocv::data::LinearTrack3DArray>(0);
    for(size_t idx=0; idx<ltrack_v.size(); ++idx)
      data.emplace_back(std::move(ltrack_v[idx]));

    return true;
  }

}
#endif
