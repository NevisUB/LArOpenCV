#ifndef DEFECTBREAKER_H
#define DEFECTBREAKER_H

#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

/*
@brief: given a contour, break that contour into atomic contours, store defect information
*/

using larocv::GEO2D_Contour_t;

namespace larocv {
  
  class DefectBreaker : public laropencv_base{
    
  public:
    
    DefectBreaker() :
      laropencv_base("DefectBreaker"),
      _min_defect_size    (kINVALID_INT),
      _hull_edge_pts_split(kINVALID_INT),
      _n_allowed_breaks   (kINVALID_INT)
    { }
    
    ~DefectBreaker(){}
    
    void Configure(const Config_t &pset);

    void Configure(int min_defect_size, int hull_edge_pts_split, int n_allowed_breaks);

    //break contour --> create atomics
    larocv::data::TrackClusterCompound
    BreakContour(const GEO2D_Contour_t& in_ctor) const;

    //break contour --> into more contours
    GEO2D_ContourArray_t
    SplitContour(const GEO2D_Contour_t& in_ctor) const;
    
  private:
    
    cv::Vec4i
    max_hull_edge(const GEO2D_Contour_t& ctor,
		  std::vector<cv::Vec4i> defects) const;
    
    bool
    on_line(const geo2d::Line<float>& line,
	    cv::Point pt) const;
    
    void
    split_contour(const GEO2D_Contour_t& ctor,
		  GEO2D_Contour_t& ctor1,
		  GEO2D_Contour_t& ctor2,
		  const geo2d::Line<float>& line) const;
    
    void
    fill_hull_and_defects(const GEO2D_Contour_t& ctor,
			  std::vector<int>& hullpts,
			  std::vector<cv::Vec4i>& defects,
			  std::vector<float>& defects_d) const;

    void
    filter_defects(std::vector<cv::Vec4i>& defects,
		   std::vector<float>& defects_d,
		   float min_defect_size) const;
    
    geo2d::Line<float>
    find_line_hull_defect(const GEO2D_Contour_t& ctor,
			  cv::Vec4i defect) const;
    
    geo2d::Line<float>
    scan_breaker(const GEO2D_Contour_t& ctor,
		 cv::Vec4i defect) const;

    void
    AssociateDefects(const data::TrackClusterCompound& cluscomp,
		     const data::AtomicContour& parent,
		     const data::ContourDefect& defect,
		     data::AtomicContour& child1,
		     data::AtomicContour& child2) const;
    
    
  private:

    int _min_defect_size;
    int _hull_edge_pts_split;
    int _n_allowed_breaks;
    
  };
}

#endif

