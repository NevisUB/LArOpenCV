#ifndef __PIZEROFILTERV_H__
#define __PIZEROFILTERV_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

#include "Vertex2D.h"

namespace larocv {
  /**
     \class PiZeroFilterV
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PiZeroFilterV : public larocv::ClusterAlgoBase {
    
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
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);



    double _max_rad_length; 
    double _width;
    double _height;
    
    double _small_dot_prod;

  private:

    Point2D backprojectionpoint(Point2D point1, Point2D point2, Point2D point3, Point2D point4);
    double distance2D(Point2D point1, Point2D point2, double width, double height);
    Cluster2DArray_t merging(Cluster2DArray_t filtered);

  };
  
  /**
     \class larocv::PiZeroFilterVFactory
     \brief A concrete factory class for larocv::PiZeroFilterV
   */
  class PiZeroFilterVFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PiZeroFilterVFactory() { AlgoFactory::get().add_factory("PiZeroFilterV",this); }
    /// dtor
    ~PiZeroFilterVFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PiZeroFilterV(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

