/**
 * \file ProjectionCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ProjectionCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PROJECTIONCLUSTER_H__
#define __PROJECTIONCLUSTER_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class ProjectionCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ProjectionCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor
    ProjectionCluster(const std::string name="ProjectionCluster") : ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ProjectionCluster(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    /// Process method
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    /// PostProcess method
    void _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    AlgorithmID_t _vtx_algo_id;
    AlgorithmID_t _cluster_algo_id;
    AlgorithmID_t _pca_algo_id;
    AlgorithmID_t _atom_ana_id;
  };

  class ProjectionClusterData : public larocv::AlgoDataBase {
  public:
    ProjectionClusterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~ProjectionClusterData(){}

    void Clear() { _nplanes = 0; }

    size_t _nplanes;
    
  };
  
  /**
     \class larocv::ProjectionClusterFactory
     \brief A concrete factory class for larocv::ProjectionCluster
   */
  class ProjectionClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ProjectionClusterFactory() { AlgoFactory::get().add_factory("ProjectionCluster",this); }
    /// dtor
    ~ProjectionClusterFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new ProjectionCluster(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new ProjectionClusterData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 

