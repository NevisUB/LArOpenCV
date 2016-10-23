/**
 * \file PCAOverall.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class BoundingRectDir
 *
 * @author XXX
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __BOUNDINGRECTDIR_H__
#define __BOUNDINGRECTDIR_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class BoundingRectDir
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class BoundingRectDir : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    BoundingRectDir(const std::string name="BoundingRectDir") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~BoundingRectDir(){}

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
    float dist(const ::cv::Point2f& p1, const ::cv::Point2f& p2);
  };
  
  /**
     \class larocv::BoundingRectDirFactory
     \brief A concrete factory class for larocv::BoundingRectDir
   */
  class BoundingRectDirFactory : public AlgoFactoryBase {
  public:
    /// ctor
    BoundingRectDirFactory() { AlgoFactory::get().add_factory("BoundingRectDir",this); }
    /// dtor
    ~BoundingRectDirFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new BoundingRectDir(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

