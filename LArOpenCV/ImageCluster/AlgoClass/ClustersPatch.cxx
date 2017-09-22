#ifndef CLUSTERSPATCH_CXX
#define CLUSTERSPATCH_CXX

#include "ClustersPatch.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "Geo2D/Core/spoon.h"
#include "Geo2D/Core/Geo2D.h"

namespace larocv {


  void ClustersPatch::Configure(const Config_t& pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
  }
  
  cv::Mat ClustersPatch::AnglePatch(const cv::Mat img){
    
    auto res_img = img.clone();
    
    GEO2D_ContourArray_t img_ctors;
    img_ctors = FindContours(res_img);

    if( !img_ctors.size() ) return res_img;
    
    for(size_t ctor_idx = 0; ctor_idx < img_ctors.size(); ++ctor_idx){
      cv::Mat masked_img = MaskImage(res_img, img_ctors[ctor_idx],0,true);
      
      geo2d::Vector<float> edge1, edge2;
      
      FindEdges(masked_img, edge1, edge2);
      //FindEdges(img_ctors[ctor_idx], edge0, edge1);
      
    }
    return res_img; 
  }
}
#endif
