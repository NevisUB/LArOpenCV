/**
 * \file StartEndMerge.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class StartEndMerge
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __STARTENDMERGE_H__
#define __STARTENDMERGE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class StartEndMerge
     @brief A simple clustering algorithm meant to serve for testing/example by vic
  */
  class StartEndMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    StartEndMerge(const std::string name="StartEndMerge") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~StartEndMerge(){}

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
    
    double _max_start_end_d;
    bool   _require_pca_check;
    double _angle_cut;
    
    double _w;
    double _h;


    void traverse(const Cluster2D& c1,         // the input cluster
		  const Cluster2DArray_t& ca,  // all the clusters
		  std::map<int,bool >& used,   //which one is used
		  const std::map<size_t,std::vector<size_t> >& cnse, //neighbors
		  std::map<size_t,std::vector<size_t> >&       cmse, //traversed and unique graph
		  int i,int k);
      
    Cluster2D join_clusters(const std::map<size_t,std::vector<size_t> >& cmse,
			    const Cluster2DArray_t& clusters,
			    int i);
    bool check_slope( const Cluster2D& c1,
		      const Cluster2D& c2 );

  };
  
  /**
     \class larocv::StartEndMergeFactory
     \brief A concrete factory class for larocv::StartEndMerge
   */
  class StartEndMergeFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    StartEndMergeFactory() { ClusterAlgoFactory::get().add_factory("StartEndMerge",this); }
    /// dtor
    ~StartEndMergeFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new StartEndMerge(instance_name); }
  };
  /// Global larocv::StartEndMergeFactory to register ClusterAlgoFactory
  static StartEndMergeFactory __global_StartEndMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

