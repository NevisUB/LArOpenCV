#ifndef __SHOWERVERTEXANALYSIS_CXX__
#define __SHOWERVERTEXANALYSIS_CXX__

#include "ShowerVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerVertexAnalysisFactory to register AlgoFactory
  static ShowerVertexAnalysisFactory __global_ShowerVertexAnalysisFactory__;

  void ShowerVertexAnalysis::Reset()
  {}
  
  void ShowerVertexAnalysis::_Configure_(const Config_t &pset) {
    Register(new data::Vertex3DArray);
  }

  
  bool ShowerVertexAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    //decide on vertex

    
    return true;
  }
   
}
#endif

