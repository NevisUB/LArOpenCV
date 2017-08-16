#ifndef __CHEAPVERTEXFINDER_CXX__
#define __CHEAPVERTEXFINDER_CXX__

#include "CheapVertexFinder.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::CheapVertexFinderFactory to register AlgoFactory
  static CheapVertexFinderFactory __global_CheapVertexFinderFactory__;
  
  void CheapVertexFinder::_Configure_(const Config_t &pset)
  {
    // Register 3 particle arrays, 1 per plane
    Register(new data::ParticleClusterArray);
    Register(new data::ParticleClusterArray);
    Register(new data::ParticleClusterArray);
  }

  bool CheapVertexFinder::_PostProcess_() const
  { return true; }

  void CheapVertexFinder::_Process_() {
    
    // Get the track and shower image
    auto shower_img_v = ImageArray(ImageSetID_t::kImageSetShower);
    auto track_img_v  = ImageArray(ImageSetID_t::kImageSetTrack);

    // resize _closest_pt vector to size=3 (1 point per plane)
    _closest_pt_v.resize(3);

    // find & store the closest point between track and shower particles per plane
    

    
    
    
    
    return;
  }
}
#endif
