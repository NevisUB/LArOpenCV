#ifndef DEFECTBREAKER_H
#define DEFECTBREAKER_H

#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "LArOpenCV/Core/laropencv_logger.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

using larocv::GEO2D_Contour_t;

namespace larocv {
  
  class DefectBreaker{
    
  public:
    
    DefectBreaker()   :
      _min_defect_size    (larocv::kINVALID_INT),
      _hull_edge_pts_split(larocv::kINVALID_INT),
      _n_allowed_breaks   (larocv::kINVALID_INT),
      _logger(nullptr)
      
    { _logger = &(larocv::logger::get("DBLogger")); }
    
    ~DefectBreaker(){}
    
    void Configure(const Config_t &pset);


    //break contour --> create atomics
    larocv::data::TrackClusterCompound
    BreakContour(const GEO2D_Contour_t& in_ctor);

    
    inline const larocv::logger& logger() const
    { return *_logger; }
    
  private:
    
    cv::Vec4i
    max_hull_edge(const GEO2D_Contour_t& ctor,
		  std::vector<cv::Vec4i> defects);
    
    bool
    on_line(const geo2d::Line<float>& line,
	    cv::Point pt);
    
    void
    split_contour(const GEO2D_Contour_t& ctor,
		  GEO2D_Contour_t& ctor1,
		  GEO2D_Contour_t& ctor2,
		  const geo2d::Line<float>& line);
    
    void
    fill_hull_and_defects(const GEO2D_Contour_t& ctor,
			  std::vector<int>& hullpts,
			  std::vector<cv::Vec4i>& defects,
			  std::vector<float>& defects_d);

    void
    filter_defects(std::vector<cv::Vec4i>& defects,
		   std::vector<float>& defects_d,
		   float min_defect_size);
    
    geo2d::Line<float>
    find_line_hull_defect(const GEO2D_Contour_t& ctor,
			  cv::Vec4i defect);
    
    geo2d::Line<float>
    scan_breaker(const GEO2D_Contour_t& ctor,
		 cv::Vec4i defect);

    void
    AssociateDefects(const data::TrackClusterCompound& cluscomp,
		     const data::AtomicContour& parent,
		     const data::ContourDefect& defect,
		     data::AtomicContour& child1,
		     data::AtomicContour& child2);
    
    
  private:

    int _min_defect_size;
    int _hull_edge_pts_split;
    int _n_allowed_breaks;
    
    larocv::logger *_logger;
    
  };
}

#endif

