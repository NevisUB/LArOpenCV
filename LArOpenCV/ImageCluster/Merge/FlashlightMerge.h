/**
 * \file TriangleClusterExt.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TriangleClusterExt
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __FLASHLIGHTMERGE_H__
#define __FLASHLIGHTMERGE_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class FlashlightMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class FlashlightMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    FlashlightMerge(const std::string name="FlashlightMerge") :
      ClusterAlgoBase(name)
      ,_trilen(50)
      ,_triangle(20)
    {}
    
    /// Default destructor
    ~FlashlightMerge(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:
    int _N;
    float _trilen;
    float _triangle;
    template <typename T> inline float sgn(T val) { return (T(0) < val) - (val < T(0)); }
    void unravel(std::vector<bool>& used,   //which one is used
		 const std::map<int,std::vector<int> >& cnse, //neighbors
		 std::vector< std::vector<int> >&       cmse, //traversed and unique graph
		 int i,int k);
    float d_to_vtx(const Contour_t& ctor,
		   const Point2D& vtx);

    bool line_intersection(const ::cv::Point& p1,
			   const ::cv::Point& p2,
			   const ::cv::Point& p3,
			   const ::cv::Point& p4);
    bool overlap(const Contour_t& c1,
		 const Contour_t& c2);
  };
  
  /**
     \class larocv::FlashlightMergeFactory
     \brief A concrete factory class for larocv::FlashlightMerge
   */
  class FlashlightMergeFactory : public AlgoFactoryBase {
  public:
    /// ctor
    FlashlightMergeFactory() { AlgoFactory::get().add_factory("FlashlightMerge",this); }
    /// dtor
    ~FlashlightMergeFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new FlashlightMerge(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

