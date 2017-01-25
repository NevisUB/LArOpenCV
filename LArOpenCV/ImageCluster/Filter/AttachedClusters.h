/**
 * \file AttachedClusters.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class AttachedClusters
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __ATTACHEDCLUSTERS_H__
#define __ATTACHEDCLUSTERS_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class AttachedClusters
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class AttachedClusters : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    AttachedClusters(const std::string name="AttachedClusters") :
      ClusterAlgoBase(name)
      ,_max_defect_size(99999)
    {}
    
    /// Default destructor
    ~AttachedClusters(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:

    int _max_defect_size;
    float _hull_contour_area_ratio;
    
  };
  
  /**
     \class larocv::AttachedClustersFactory
     \brief A concrete factory class for larocv::AttachedClusters
   */
  class AttachedClustersFactory : public AlgoFactoryBase {
  public:
    /// ctor
    AttachedClustersFactory() { AlgoFactory::get().add_factory("AttachedClusters",this); }
    /// dtor
    ~AttachedClustersFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new AttachedClusters(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

