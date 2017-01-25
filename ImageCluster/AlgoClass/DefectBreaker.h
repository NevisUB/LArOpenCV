#ifndef DEFECTBREAKER_H
#define DEFECTBREAKER_H

#include "FhiclLite/PSet.h"
#include "Core/Line.h"
#include "Core/LineSegment.h"
#include "Core/laropencv_logger.h"
#include "AlgoData/DefectClusterData.h"

using larocv::GEO2D_Contour_t;

class DefectBreaker{

public:

  DefectBreaker()   :
    _min_defect_size    (larocv::kINVALID_INT),
    _hull_edge_pts_split(larocv::kINVALID_INT),
    _n_allowed_breaks   (larocv::kINVALID_INT),
    _logger(nullptr)
  { _logger = &(larocv::logger::get("DBLogger")); }
  
  ~DefectBreaker(){}

  void Configure(const fcllite::PSet &pset);

  //split contour --> break into atomics and define edges
  larocv::data::ClusterCompound SplitContour(const GEO2D_Contour_t& in_ctor);

  //break contour --> create atomics
  larocv::data::ClusterCompound BreakContour(const GEO2D_Contour_t& in_ctor);
  
  void AssociateDefects(const larocv::data::ClusterCompound& cluscomp,
			const larocv::data::AtomicContour& parent,
			const larocv::data::ContourDefect& defect,
			larocv::data::AtomicContour& child1,
			larocv::data::AtomicContour& child2);

  double DistanceAtom2Point(const larocv::data::AtomicContour& atom, const geo2d::Vector<float>& point) const;

  geo2d::Vector<float> ChooseStartPoint(larocv::data::ClusterCompound& cluscomp);
  
  std::vector<size_t> OrderAtoms(const larocv::data::ClusterCompound& cluster,
				 const geo2d::Vector<float>& start_) const;
  
  std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
  AtomsEdge(const larocv::data::ClusterCompound& cluster,
	    const geo2d::Vector<float>& start_,
	    const std::vector<size_t> atom_order_v) const;
  
  
  inline const larocv::logger& logger() const
  { return *_logger; }
  
private:
  
  cv::Vec4i max_hull_edge(const GEO2D_Contour_t& ctor, std::vector<cv::Vec4i> defects);

  bool on_line(const geo2d::Line<float>& line,cv::Point pt);
  void split_contour(const GEO2D_Contour_t& ctor, GEO2D_Contour_t& ctor1, GEO2D_Contour_t& ctor2, const geo2d::Line<float>& line);
  void fill_hull_and_defects(const GEO2D_Contour_t& ctor,
			     std::vector<int>& hullpts,
			     std::vector<cv::Vec4i>& defects,
			     std::vector<float>& defects_d);
  void filter_defects(std::vector<cv::Vec4i>& defects,
		      std::vector<float>& defects_d,
		      float min_defect_size);
  geo2d::Line<float> find_line_hull_defect(const GEO2D_Contour_t& ctor, cv::Vec4i defect);
  geo2d::Line<float> scan_breaker(const GEO2D_Contour_t& ctor, cv::Vec4i defect);
  
private:
  
  int _min_defect_size;
  int _hull_edge_pts_split;
  int _n_allowed_breaks;

  larocv::logger *_logger;
  
};


#endif

