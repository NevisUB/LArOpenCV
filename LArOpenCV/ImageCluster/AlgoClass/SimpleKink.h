#ifndef SIMPLEKINK_H
#define SIMPLEKINK_H

#include <iostream>
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/Base/BaseUtil.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterBase.h"

#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"


namespace larocv {
  
  class SimpleKink : public laropencv_base {

  public:

    /// Default constructor
    SimpleKink();

    /// Default destructor
    ~SimpleKink(){}

    void Configure(const Config_t &pset);

    std::vector<geo2d::Vector<float>> FindInflections(const GEO2D_Contour_t &ctor, const cv::Mat &img);


  private:

    int _log_level;
    uint _subpath_step_size;
    float _min_path_ratio;
    uint _chunkiness;
 
  
  };

}
#endif

