#ifndef __RECREATEVERTEX_H__
#define __RECREATEVERTEX_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexScan3D.h"

namespace larocv {

  class ReCreateVertex : public laropencv_base {
    
  public:
  ReCreateVertex() : laropencv_base("ReCreateVertex") {}
    ~ReCreateVertex() {}
    
    void Configure(const Config_t &pset);
    void ReCreate(data::Vertex3D& vertex,const std::vector<cv::Mat>& thresh_img_v);
    void SetPlaneInfo(const ImageMeta& meta);

  private:
    float _default_radius;
    bool _scan_radius;
    LArPlaneGeo _geo;
    VertexScan3D _VertexScan3D;

  };
}

#endif
/** @} */ // end of doxygen group

