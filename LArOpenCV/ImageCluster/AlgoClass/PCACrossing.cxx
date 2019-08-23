#ifndef __PCACROSSING_CXX__
#define __PCACROSSING_CXX__

#include "PCACrossing.h"
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
#endif
#include "LArOpenCV/Core/larbys.h"
#include "Geo2D/Core/Geo2D.h"

using larocv::larbys;

namespace larocv {

  PCACrossing::PCACrossing()
  {
    _dist_to_pixel = 2.0;
    _filter_intersections = true;
    _filter_px_val_thresh = 10.0;
  }

  void PCACrossing::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _dist_to_pixel = pset.get<float>("DistToPixel",2.0);
    _filter_intersections = pset.get<bool>("FilterXs",true);
    _filter_px_val_thresh = pset.get<int>("FilterPxValThresh",10.0);
  }

  std::vector<geo2d::Line<float> >
  PCACrossing::ComputePCALines(const std::vector<GEO2D_Contour_t>& cluscomp) {

    std::vector<geo2d::Line<float> > line_v;
    line_v.reserve(cluscomp.size());
    
    for(const auto& atomic : cluscomp)
      line_v.emplace_back(CalcPCA(atomic));

    return line_v;
  }

  std::vector<geo2d::Line<float> >
  PCACrossing::ComputePCALines(const data::TrackClusterCompound& cluscomp) {
    
    std::vector<geo2d::Line<float> > line_v;
    line_v.reserve(cluscomp.size());
    
    for(const auto& atomic : cluscomp)
      line_v.emplace_back(CalcPCA(atomic));

    return line_v;
  }


  std::vector<geo2d::Vector<float> >
  PCACrossing::ComputeIntersections(const std::vector<geo2d::Line<float> >& line_v,
				    const cv::Mat& img) {

    LAROCV_DEBUG() << "Computing intersections for " << line_v.size() << " lines" << std::endl;
    LAROCV_DEBUG() << "Will filter based on nonzero pix? : " << _filter_intersections << std::endl;
    
    std::vector<geo2d::Vector<float> > intersections_v;

    intersections_v.reserve(line_v.size() * line_v.size());
    
    for(size_t i=0; i<line_v.size(); ++i) {
      auto const& line1 = line_v[i];
      
      for(size_t j=i+1; j<line_v.size(); ++j) {
	auto const& line2 = line_v[j];
	
	auto ipoint = geo2d::IntersectionPoint(line1,line2);

	intersections_v.emplace_back(std::move(ipoint));
	
      }
    }

    LAROCV_DEBUG() << "Got " << intersections_v.size() << " intersections" << std::endl;
    
    if (_filter_intersections)
      FilterIntersections(intersections_v, img);

    LAROCV_DEBUG() << "After filter have " << intersections_v.size() << " intersections" << std::endl;
    
    return intersections_v;
  }
  
  std::vector<geo2d::Vector<float> >
  PCACrossing::ComputeIntersections(const std::vector<GEO2D_Contour_t>& cluscomp,
				    const cv::Mat& img) {
    
    LAROCV_DEBUG() << "Gettting intersections from PCA of incoming contours" << std::endl;
    
    auto line_v = ComputePCALines(cluscomp);

    return ComputeIntersections(line_v,img);
  }

  std::vector<geo2d::Vector<float> >
  PCACrossing::ComputeIntersections(const data::TrackClusterCompound& cluscomp,
				    const cv::Mat& img) {
    
    LAROCV_DEBUG() << "Gettting intersections from PCA of incoming contours" << std::endl;
    
    auto line_v = ComputePCALines(cluscomp);
    
    return ComputeIntersections(line_v,img);
  }

  
  void
  PCACrossing::FilterIntersections(std::vector<geo2d::Vector<float> >& pts_v,
				   const cv::Mat& img) {
    
    std::vector<geo2d::Vector<float> > pts_tmp_v;
    pts_tmp_v.reserve(pts_v.size());
    
    auto thresh_img = img.clone();
    if (_filter_px_val_thresh > 0)
      cv::threshold(img,thresh_img,_filter_px_val_thresh,255,CV_THRESH_BINARY);
    
    //get the nonzero pixel values
    GEO2D_Contour_t nonzero_v;
    findNonZero(thresh_img,nonzero_v);
    
    LAROCV_DEBUG() << "Found " << nonzero_v.size() << " non zero points" << std::endl;

    for(const auto& ipoint : pts_v ) {
      LAROCV_DEBUG() << "Examining pt: " << ipoint << std::endl;
      for(const auto& pt : nonzero_v) {
	geo2d::Vector<float> pt_f = pt;
	if ( geo2d::dist(pt_f,ipoint) < _dist_to_pixel  ) {
	  LAROCV_DEBUG() << "Matched with " << pt_f << std::endl;
	  pts_tmp_v.emplace_back(std::move(ipoint));
	  break;
	} 
      }
    }
    
    std::swap(pts_tmp_v,pts_v);
  }
  
}

#endif
