#ifndef __TRACKANALYSIS_CXX__
#define __TRACKANALYSIS_CXX__

#include "TrackAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::TrackAnalysisFactory to register AlgoFactory
  static TrackAnalysisFactory __global_TrackAnalysisFactory__;

  void TrackAnalysis::Reset()
  {}
  
  void TrackAnalysis::_Configure_(const Config_t &pset) {
    Register(new data::Vertex3DArray);
  }

  
  bool TrackAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    //decide on vertex

    
    return true;
  }
   
}
#endif

