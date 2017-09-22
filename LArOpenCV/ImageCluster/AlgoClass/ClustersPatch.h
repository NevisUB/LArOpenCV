#ifndef CLUSTERSPATCH_H
#define CLUSTERSPATCH_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SuperClusterer.h"
#include "Geo2D/Core/Geo2D.h"
/*
  @brief: the class is supposed to do contour/img analysis to merge broken tracks into one. 
*/


namespace larocv {

  class ClustersPatch : public laropencv_base {
  
  public:
    
    /// Default constructor
  ClustersPatch() : 
    laropencv_base("ClustersPatch")
      {}
    
    /// Default destructor
    ~ClustersPatch(){}
    
    void Configure(const Config_t& pset);

    cv::Mat AnglePatch(const cv::Mat);
    
    
  private:
    SuperClusterer _SuperClusterer;
  };
}

#endif
/** @} */ // end of doxygen group 

