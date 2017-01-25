/**
 * \file VertexInHull.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class VertexInHull
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __VERTEXINHULL_H__
#define __VERTEXINHULL_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class VertexInHull
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class VertexInHull : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    VertexInHull(const std::string name="VertexInHull") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~VertexInHull(){}

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

  };
  
  /**
     \class larocv::VertexInHullFactory
     \brief A concrete factory class for larocv::VertexInHull
   */
  class VertexInHullFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexInHullFactory() { AlgoFactory::get().add_factory("VertexInHull",this); }
    /// dtor
    ~VertexInHullFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new VertexInHull(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

