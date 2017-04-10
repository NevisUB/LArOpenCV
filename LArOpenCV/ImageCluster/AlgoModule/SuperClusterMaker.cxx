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
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));

    for(size_t plane=0;plane<3;++plane)
      Register(new data::ParticleClusterArray); // plane X
  }

  void SuperClusterMaker::_Process_(const Cluster2DArray_t& clusters,
				    ::cv::Mat& img,
				    ImageMeta& meta,
				    ROI& roi)
  {
    auto& par_v = AlgoData<data::ParticleClusterArray>(meta.plane());
    
    GEO2D_ContourArray_t ctor_v;

    _SuperClusterer.CreateSuperCluster(img,ctor_v);

    for(auto& ctor : ctor_v) {
      data::ParticleCluster pc;
      pc._ctor=std::move(ctor);
      par_v.emplace_back(std::move(pc));
    }

    LAROCV_DEBUG() << "Found " << par_v.as_vector().size()
		   << " super particles on plane " << meta.plane() << std::endl;
  }

}
#endif
