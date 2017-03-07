#ifndef __SHOWERONEDGE_CXX__
#define __SHOWERONEDGE_CXX__

#include "ShowerOnEdge.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include <array>

namespace larocv {

  /// Global larocv::ShowerOnEdgeFactory to register AlgoFactory
  static ShowerOnEdgeFactory __global_ShowerOnEdgeFactory__;

  void ShowerOnEdge::Reset()
  {}
  
  void ShowerOnEdge::_Configure_(const Config_t &pset) {
    Register(new data::Vertex3DArray);
  }
  
  void ShowerOnEdge::_Process_(const larocv::Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       larocv::ImageMeta& meta,
			       larocv::ROI& roi)
  {
    _geo.ResetPlaneInfo(meta);
  }
  
  bool ShowerOnEdge::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "start" << std::endl;
    auto num_planes = img_v.size();
    auto& ass_man = AssManager();

    // My 3D vertex
    auto& vertex3d_v  = AlgoData<data::Vertex3DArray>(0);
    
    
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
}
#endif

