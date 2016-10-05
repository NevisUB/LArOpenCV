#ifndef __ALGO_H__
#define __ALGO_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
 
  class Algo : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    Algo(const std::string name = "Algo") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~Algo(){}
    
    /// Finalize after process
    void Finalize(TFile*) {}

    /// functions to test out algo in ipython notebook

    // ContourArray_t& f_mip_ctor_v(){return _mip_ctor_v;}
    // ContourArray_t& f_hip_ctor_v(){return _hip_ctor_v;}
    // ContourArray_t& f_all_ctor_v(){return _all_ctor_v;}
    // std::vector<std::vector<int> >& f_hullpts_v() {return _hullpts_v;}
    // std::vector<std::vector<::cv::Vec4i> >& f_defects_v() {return _defects_v;}

    ContourArray_t _mip_ctor_v;
    ContourArray_t _hip_ctor_v;
    ContourArray_t _all_ctor_v;
    std::vector<std::vector<int> > _hullpts_v;
    std::vector<std::vector<::cv::Vec4i> > _defects_v;
    std::vector< std::vector<std::pair<::cv::Point2f,::cv::Point2f> > > _split_defects_v;
    Cluster2DArray_t _ocluster_v;
  protected:
    
    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi);
    
  private:


    
    void clear();
    
    bool test_point_above(std::pair<::cv::Point2f,::cv::Point2f> segment,::cv::Point2f pt);

    int four_pt_intersect(::cv::Point2f p1,
			   ::cv::Point2f p2,
			   ::cv::Point2f p3,
			   ::cv::Point2f p4);
    ::cv::Point2f intersect(float x1,float y1,
			    float x2,float y2,
			    float x3,float y3);
    double intersect_distance(float x1,float y1,float x2,float y2,float x3,float y3);
    ::cv::Point2f intersection_point(float x1,float x2,float y1,float y2,float x3,float x4,float y3,float y4);
    
   
    double distance(float x1,float x2,float y1,float y2);
    void FillClusterParams(Cluster2DArray_t& cluster2d_v,const ::cv::Mat& img);

    int _min_hip_cluster_size;
    int _min_mip_cluster_size;
    int _min_defect_size;
    int _hull_edge_pts_split;
    int _mip_thresh;
    int _hip_thresh;
  };
  
  /**
     \class larocv::AlgoFactory
     \brief A concrete factory class for larocv::Algo
   */
  class AlgoFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    AlgoFactory() { ClusterAlgoFactory::get().add_factory("Algo",this); }
    /// dtor
    ~AlgoFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new Algo(instance_name); }
  };
  /// Global larocv::AlgoFactory to register ClusterAlgoFactory
  static AlgoFactory __global_AlgoFactory__;
  
}
#endif
/** @} */ // end of doxygen group 


