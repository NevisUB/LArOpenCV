#ifndef __TRACKANALYSIS_CXX__
#define __TRACKANALYSIS_CXX__

#include "TrackAnalysis.h"

namespace larocv {

  /// Global TrackAnalysisFactory to register AlgoFactory
  static TrackAnalysisFactory __global_TrackAnalysisFactory__;

  void TrackAnalysis::_Configure_(const Config_t &pset)
  {
    _SuperClusterer.set_verbosity(this->logger().level());
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));
    Register(new data::ContourArrayData); // plane 0
    Register(new data::ContourArrayData); // plane 1
    Register(new data::ContourArrayData); // plane 2
  }

  void TrackAnalysis::_Process_(const Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				ImageMeta& meta,
				ROI& roi)
  {
    auto& data = AlgoData<data::ContourArrayData>(meta.plane());

    _SuperClusterer.CreateSuperCluster(img,data);
    
  }
  

}
#endif
