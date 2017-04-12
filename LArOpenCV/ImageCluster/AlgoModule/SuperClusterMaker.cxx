#ifndef __SUPERCLUSTERMAKER_CXX__
#define __SUPERCLUSTERMAKER_CXX__

#include "SuperClusterMaker.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  /// Global SuperClusterMakerFactory to register AlgoFactory
  static SuperClusterMakerFactory __global_SuperClusterMakerFactory__;

  void SuperClusterMaker::_Configure_(const Config_t &pset)
  {
    _SuperClusterer.set_verbosity(this->logger().level());

    // Configure the SuperCluster finding algorithm via pset
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));

    // Register per plane particle clusters
    for(size_t plane=0;plane<3;++plane)
      Register(new data::ParticleClusterArray); // plane X
  }

  bool SuperClusterMaker::_PostProcess_() const
  { return true; }
  
  void SuperClusterMaker::_Process_() 
  {
    //
    // 0) Prep
    //
    auto img_v = ImageArray();
    auto const& meta_v = MetaArray();

    //
    // 1) Per plane, find super contours in the ADC image
    //
    for(size_t img_idx=0; img_idx<img_v.size(); ++img_idx) {

      // Get this planes image
      auto& img = img_v[img_idx];
      auto const& meta = meta_v.at(img_idx);

      // Get AlgoData output, an array of ParticleCluster per plane
      auto& par_v = AlgoData<data::ParticleClusterArray>(meta.plane());
      
      GEO2D_ContourArray_t ctor_v;
      _SuperClusterer.CreateSuperCluster(img,ctor_v);
      
      // Make a particle cluster for each found contour
      for(auto& ctor : ctor_v) {
	data::ParticleCluster pc;
	pc._ctor=std::move(ctor);
	par_v.emplace_back(std::move(pc));
      }
      
      LAROCV_DEBUG() << "Found " << par_v.as_vector().size()
		     << " super particles on plane " << meta.plane() << std::endl;
    }
  }

}
#endif
