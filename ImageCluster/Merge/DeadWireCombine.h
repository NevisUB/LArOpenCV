/**
 * \file DeadWireCombine.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class DeadWireCombine
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __DEADWIRECOMBINE_H__
#define __DEADWIRECOMBINE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class DeadWireCombine
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */

  struct paired_point {
    
    ::cv::Point p1;
    ::cv::Point p2;

    float d;
    
    const Cluster2D* c1;
    const Cluster2D* c2;

    size_t i1;
    size_t i2;

  };
  
  class DeadWireCombine : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    DeadWireCombine(const std::string name="DeadWireCombine") :
      ClusterAlgoBase(name)
      , _min_width(0)
      , _closeness(0)
      , _tolerance(0.0)
      
    { _wires_v.resize(3); _loaded = {false,false,false}; }
    
    /// Default destructor
    ~DeadWireCombine(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:
    
    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta);
    
  private:

    // min wire width to care about this algo
    int _min_width;
    int _closeness;
    std::vector<bool> _loaded;

    double _tolerance;
    
    //per plane //set of wire //ranges
    std::vector<std::vector<std::pair<int,int> > > _wires_v; 

    paired_point min_dist(const Cluster2D& c1,const Cluster2D& c2 );

    Cluster2D join_clusters(const Cluster2D& c1, const Cluster2D& c2);
    
    inline float dist(float x1, float x2, float y1, float y2 )
    { return std::sqrt( (x1-x2)*(x1-x2) + (y2-y1)*(y2-y1) ); }

    std::vector<std::pair<int,int> > LoadWires(ImageMeta& meta);
    
  };
  
  /**
     \class larocv::DeadWireCombineFactory
     \brief A concrete factory class for larocv::DeadWireCombine
   */
  class DeadWireCombineFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    DeadWireCombineFactory() { ClusterAlgoFactory::get().add_factory("DeadWireCombine",this); }
    /// dtor
    ~DeadWireCombineFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new DeadWireCombine(instance_name); }
  };
  /// Global larocv::DeadWireCombineFactory to register ClusterAlgoFactory
  static DeadWireCombineFactory __global_DeadWireCombineFactory__;
}
#endif
/** @} */ // end of doxygen group 

