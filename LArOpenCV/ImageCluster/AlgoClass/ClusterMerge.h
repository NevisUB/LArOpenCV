#ifndef __CLUSTERMERGE_H__
#define __CLUSTERMERGE_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

/*
  @brief: XXX
*/

namespace larocv {
 
  class ClusterMerge : public laropencv_base {
    
  public:
    ClusterMerge(){};
    ~ClusterMerge(){}

    void Configure(const Config_t &pset);


    GEO2D_ContourArray_t
    GenerateFlashlights(geo2d::Vector<float> startpt,
			const GEO2D_ContourArray_t& super_ctor_v);

    size_t
    StartIndex(const GEO2D_ContourArray_t& super_ctor_v,
	       const GEO2D_Contour_t& parent_ctor);
    
    GEO2D_Contour_t
    MergeFlashlights(const GEO2D_ContourArray_t& flashlight_v,
		     size_t start_index);
    
    int _N;
    float _trilen;
    float _triangle;
    
  private:
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

