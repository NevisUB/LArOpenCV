/**
 * \file ClusterWriteOut.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ClusterWriteOut
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CLUSTERWRITEOUT_H__
#define __CLUSTERWRITEOUT_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class ClusterWriteOut
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ClusterWriteOut : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    ClusterWriteOut(const std::string name="ClusterWriteOut") :
      ClusterAlgoBase(name)
    {}    
    
    /// Default destructor
    ~ClusterWriteOut(){}

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
    void clear_ss(::std::stringstream& ss);

  };
  
  /**
     \class larocv::ClusterWriteOutFactory
     \brief A concrete factory class for larocv::ClusterWriteOut
   */
  class ClusterWriteOutFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ClusterWriteOutFactory() { AlgoFactory::get().add_factory("ClusterWriteOut",this); }
    /// dtor
    ~ClusterWriteOutFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new ClusterWriteOut(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

