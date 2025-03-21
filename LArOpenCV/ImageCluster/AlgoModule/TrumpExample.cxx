#ifndef __TRUMPEXAMPLE_CXX__
#define __TRUMPEXAMPLE_CXX__

#include "TrumpExample.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::TrumpExampleFactory to register AlgoFactory
  static TrumpExampleFactory __global_TrumpExampleFactory__;
  
  void TrumpExample::_Configure_(const Config_t &pset)
  {
    // Register 3 particle arrays, 1 per plane
    Register(new data::ParticleClusterArray);
    Register(new data::ParticleClusterArray);
    Register(new data::ParticleClusterArray);
  }

  bool TrumpExample::_PostProcess_() const
  { return true; }

  void TrumpExample::_Process_() {
    
    //
    // Get the image from ID set in the configuration file
    //
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();

    //
    // Lets do the equivalent operation as in the notebook, but find his hair on 3 planes
    //

    // Get the shower image
    auto shower_img_v = ImageArray(ImageSetID_t::kImageSetShower);
    auto track_img_v  = ImageArray(ImageSetID_t::kImageSetTrack);

    for(size_t plane=0; plane < img_v.size(); ++plane) {
      auto& shower_img = shower_img_v[plane];

      // Threshold the image
      auto shower_thresh = Threshold(shower_img,10,255);

      // Find contours
      auto shower_ctor_v = FindContours(shower_thresh);

      LAROCV_DEBUG() << "I found " << shower_ctor_v.size() << " shower contours on plane " << plane << std::endl;

      // Get a reference to the AlgoModule for this plane (an array of ParticleClusters)
      auto& par_algo_data = AlgoData<data::ParticleClusterArray>(plane);

      // Place the found contours in a ParticleCluster      
      for(auto&	ctor : shower_ctor_v) {
	data::ParticleCluster pcluster;
	pcluster._ctor = std::move(ctor);
	par_algo_data.emplace_back(std::move(pcluster));
      }
      
    }
    
 
  }
}
#endif
