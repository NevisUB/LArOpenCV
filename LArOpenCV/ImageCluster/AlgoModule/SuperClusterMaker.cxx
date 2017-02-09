#ifndef __SUPERCLUSTERMAKER_CXX__
#define __SUPERCLUSTERMAKER_CXX__

#include "SuperClusterMaker.h"
#include "LArOpenCV/ImageCluster/AlgoData/ContourArrayData.h"

namespace larocv {

  /// Global SuperClusterMakerFactory to register AlgoFactory
  static SuperClusterMakerFactory __global_SuperClusterMakerFactory__;

  void SuperClusterMaker::_Configure_(const Config_t &pset)
  {
    _SuperClusterer.set_verbosity(this->logger().level());
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));
    Register(new data::ContourArrayData); // plane 0
    Register(new data::ContourArrayData); // plane 1
    Register(new data::ContourArrayData); // plane 2
  }

  void SuperClusterMaker::_Process_(const Cluster2DArray_t& clusters,
				    const ::cv::Mat& img,
				    ImageMeta& meta,
				    ROI& roi)
  {
    auto& data = AlgoData<data::ContourArrayData>(meta.plane());
    
    _SuperClusterer.CreateSuperCluster(img,data);
    
  }
  

}
#endif
