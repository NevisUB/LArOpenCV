#ifndef __SHOWERVERTEXANALYSIS_CXX__
#define __SHOWERVERTEXANALYSIS_CXX__

#include "ShowerVertexAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerVertexAnalysisFactory to register AlgoFactory
  static ShowerVertexAnalysisFactory __global_ShowerVertexAnalysisFactory__;

  void ShowerVertexAnalysis::Reset()
  {}
  
  void ShowerVertexAnalysis::_Configure_(const Config_t &pset) {
  }
  
  bool ShowerVertexAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "start" << std::endl;
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
   
}
#endif

