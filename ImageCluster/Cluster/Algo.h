#ifndef __ALGO_H__
#define __ALGO_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "VicData.h"

namespace larocv {
  
  struct Line {

    Line(float s, float o) { slope=s; offset=o; }
    ~Line(){}
    
    float slope;
    float offset;
    
  };
  
  
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


    cv::Vec4i max_hull_edge(const Contour_t& ctor, std::vector<cv::Vec4i> defects);
    
    void clear();
    
    bool test_point_above(std::pair<::cv::Point2f,::cv::Point2f> segment,::cv::Point2f pt);

    bool test_point_above(const Line& line,::cv::Point pt);
    
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

    void split_contour(const Contour_t& ctor,Contour_t& ctor1,Contour_t& ctor2, const Line& line);
    
    void fill_hull_and_defects(const Contour_t& ctor,
			       std::vector<int>& hullpts,
			       std::vector<cv::Vec4i>& defects,
			       std::vector<float>& defects_d);

    void filter_defects(std::vector<cv::Vec4i>& defects,
			std::vector<float>& defects_d,
			float min_defect_size);
    Line find_line_hull_defect(const Contour_t& ctor, cv::Vec4i defect);
    
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
    /// create algo data method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new VicData(instance_name,id);}
  };
  /// Global larocv::AlgoFactory to register ClusterAlgoFactory
  static AlgoFactory __global_AlgoFactory__;
  
}
#endif
/** @} */ // end of doxygen group 


