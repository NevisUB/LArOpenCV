#ifndef __PIZEROFILTERV_H__
#define __PIZEROFILTERV_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

#include "Vertex2D.h"

namespace larcv {
  /**
     \class PiZeroFilterV
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PiZeroFilterV : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PiZeroFilterV(const std::string name="PiZeroFilterV") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PiZeroFilterV(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

    std::vector<std::vector<Vertex2D> > _verts_v;
    
  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);


    int _nhits_cut;
    double _back_startPt;  
    double _min_area;
    double _max_rad_length; 
    double _width;
    double _height;
    

    
    bool  _attempt_merging;
    double _small_dot_prod;

  private:

    Point2D backprojectionpoint(Point2D point1, Point2D point2, Point2D point3, Point2D point4);
    double distance2D(Point2D point1, Point2D point2, double width, double height);
    Cluster2DArray_t merging(Cluster2DArray_t filtered);

  };
  
  /**
     \class larcv::PiZeroFilterVFactory
     \brief A concrete factory class for larcv::PiZeroFilterV
   */
  class PiZeroFilterVFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PiZeroFilterVFactory() { ClusterAlgoFactory::get().add_factory("PiZeroFilterV",this); }
    /// dtor
    ~PiZeroFilterVFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PiZeroFilterV(instance_name); }
  };
  /// Global larcv::PiZeroFilterVFactory to register ClusterAlgoFactory
  static PiZeroFilterVFactory __global_PiZeroFilterVFactory__;
}
#endif
/** @} */ // end of doxygen group 

