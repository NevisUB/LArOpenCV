#ifndef DEADWIREPATCH_H
#define DEADWIREPATCH_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SuperClusterer.h"
#include "Geo2D/Core/Geo2D.h"
/*
  @brief: i quit ub
*/


namespace larocv {

  struct DeadEdgePatch {
    std::vector<geo2d::Vector<float> > pt_v;
    geo2d::Line<float> local_pca;
    geo2d::Circle<float> circle;
    size_t super_id;

    void init() {
      circle.radius = (float)pt_v.size();
      circle.center.x = circle.center.y = 0;
      for(const auto& pt : pt_v) {
	circle.center.x += pt.x;
	circle.center.y += pt.y;
      }
      circle.center.x /= circle.radius;
      circle.center.y /= circle.radius;
      if(circle.radius < 10) circle.radius = 10;
    }

  };

  struct DeadWireChunk {
    std::vector<geo2d::Vector<float> > lower_pt_v;
    std::vector<geo2d::Vector<float> > upper_pt_v;
  };

  class DeadWirePatch : public laropencv_base {
    
  public:
    
    /// Default constructor
  DeadWirePatch() : 
    laropencv_base("DeadWirePatch"),
      _bandaid(false), 
      _bondage(false)
	{}
    
    /// Default destructor
    ~DeadWirePatch(){}

    void Configure(const Config_t& pset);


    cv::Mat DeadPatch(const cv::Mat&img,
		      const cv::Mat&dead_ch_img);

    void FindWireEdges(const GEO2D_Contour_t& ctor,
		       geo2d::Vector<float>& edge_low,
		       geo2d::Vector<float>& edge_high);
    
    void FindWireEdges(const cv::Mat& img,
		       geo2d::Vector<float>& edge_low,
		       geo2d::Vector<float>& edge_high);

    cv::Mat MakeDeadImage(const cv::Mat& img);
    
    cv::Mat WireBandaid(const cv::Mat& img,
			const cv::Mat& dead_ch_img);
    
    cv::Mat WireBondage(const cv::Mat& img,
			const cv::Mat& dead_ch_img);
    
    std::vector<DeadWireChunk> ScanEdgePixels(const cv::Mat& img,
					      const std::vector<std::pair<int,int> >& dead_wire_v);

    std::vector<std::pair<int,int> > GenDeadRows(const cv::Mat& dead_ch_img);

    cv::Mat FillImageGap(cv::Mat img, 
			 const geo2d::Vector<float> edge_low,
			 const geo2d::Vector<float> edge_high,
			 const GEO2D_ContourArray_t ctor_v,
			 const int upper_ctor_idx, 
			 const int lower_ctor_idx);
    
    bool _bandaid;
    bool _bondage;

  private:
    SuperClusterer _SuperClusterer;

  };
}

#endif
/** @} */ // end of doxygen group 

