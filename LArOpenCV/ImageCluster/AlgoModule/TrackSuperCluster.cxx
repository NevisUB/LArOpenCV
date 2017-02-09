#ifndef __TRACKSUPERCLUSTER_CXX__
#define __TRACKSUPERCLUSTER_CXX__

#include "TrackSuperCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/ContourArrayData.h"

namespace larocv {

  /// Global TrackSuperClusterFactory to register AlgoFactory
  static TrackSuperClusterFactory __global_TrackSuperClusterFactory__;

  void TrackSuperCluster::_Configure_(const Config_t &pset)
  {
    _SuperClusterer.set_verbosity(this->logger().level());
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));
    Register(new data::ContourArrayData); // plane 0
    Register(new data::ContourArrayData); // plane 1
    Register(new data::ContourArrayData); // plane 2
  }

  void TrackSuperCluster::_Process_(const Cluster2DArray_t& clusters,
				    const ::cv::Mat& img,
				    ImageMeta& meta,
				    ROI& roi)
  {
    auto& data = AlgoData<data::ContourArrayData>(meta.plane());

    _SuperClusterer.CreateSuperCluster(img,data);
    
  }
  

}
#endif
