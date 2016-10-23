/**
 * \file CheckAlignment.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CheckAlignment
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHECKALIGNMENT_H__
#define __CHECKALIGNMENT_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class CheckAlignment
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CheckAlignment : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    CheckAlignment(const std::string name="CheckAlignment") : ClusterAlgoBase(name), _ratio_cut(0.6), _min_dist(12.)
    {}
    
    /// Default destructor
    ~CheckAlignment(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);
    
  private:

    float _ratio_cut;
    float _min_dist ;
    bool _use_start_end;
    
  };
  
  /**
     \class larocv::CheckAlignmentFactory
     \brief A concrete factory class for larocv::CheckAlignment
   */
  class CheckAlignmentFactory : public AlgoFactoryBase {
  public:
    /// ctor
    CheckAlignmentFactory() { AlgoFactory::get().add_factory("CheckAlignment",this); }
    /// dtor
    ~CheckAlignmentFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new CheckAlignment(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

