#ifndef __SHOWERANALYSIS_CXX__
#define __SHOWERANALYSIS_CXX__

#include "ShowerAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerAnalysisFactory to register AlgoFactory
  static ShowerAnalysisFactory __global_ShowerAnalysisFactory__;

  void ShowerAnalysis::Reset()
  {}
  
  void ShowerAnalysis::_Configure_(const Config_t &pset) {
    Register(new data::Vertex3DArray);
  }

  
  bool ShowerAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    //decide on vertex

    
    return true;
  }
   
}
#endif

