#ifndef ATOMICANALYSIS_H
#define ATOMICANALYSIS_H

#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "LArOpenCV/Core/laropencv_logger.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/DefectClusterData.h"

using larocv::GEO2D_Contour_t;

namespace larocv {
  
  class AtomicAnalysis{
    
  public:
    
    AtomicAnalysis() :
      _logger(nullptr)
    { _logger = &(larocv::logger::get("DBLogger")); }
    
    ~AtomicAnalysis(){}
    
    void Configure(const Config_t &pset);

    void
    AssociateDefects(const larocv::data::ClusterCompound& cluscomp,
		     const larocv::data::AtomicContour& parent,
		     const larocv::data::ContourDefect& defect,
		     larocv::data::AtomicContour& child1,
		     larocv::data::AtomicContour& child2);
    
    double
    DistanceAtom2Point(const larocv::data::AtomicContour& atom,
		       const geo2d::Vector<float>& point) const;
    
    geo2d::Vector<float>
    ChooseStartPoint(larocv::data::ClusterCompound& cluscomp);
    
    std::vector<size_t>
    OrderAtoms(const larocv::data::ClusterCompound& cluster,
	       const geo2d::Vector<float>& start_) const;
    
    std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
    AtomsEdge(const larocv::data::ClusterCompound& cluster,
	      const geo2d::Vector<float>& start_,
	      const std::vector<size_t> atom_order_v) const;
    
    // std::vector<float>
    // AtomdQdX(const cv::Mat& img, const data::AtomicContour& atom,
    // 	     const geo2d::Line<float>& pca,
    // 	     const geo2d::Vector<float>& start,
    // 	     const geo2d::Vector<float>& end) const;
    
    inline const larocv::logger& logger() const
    { return *_logger; }
    
  private:    
    larocv::logger *_logger;

    // float _pi_threshold;
    // float _dx_resolution;
    // float _atomic_region_pad;
    // float _atomic_contour_pad;

    
  };
}

#endif

