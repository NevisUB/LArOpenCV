#ifndef __ALGO_H__
#define __ALGO_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "VicData.h"

#include "Core/Line.h"

typedef std::vector<std::vector<geo2d::Vector2D<int> > > GEO2D_ContourArray_t;
typedef std::vector<geo2d::Vector2D<int> >               GEO2D_Contour_t;


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

    GEO2D_ContourArray_t _mip_ctor_v;
    GEO2D_ContourArray_t _hip_ctor_v;
    GEO2D_ContourArray_t _all_ctor_v;
    std::vector<std::vector<int> > _hullpts_v;
    std::vector<std::vector<::cv::Vec4i> > _defects_v;
    Cluster2DArray_t _ocluster_v;
    
  protected:
    
    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi);
    
  private:
    
    
    cv::Vec4i max_hull_edge(const GEO2D_Contour_t& ctor, std::vector<cv::Vec4i> defects);
    
    void clear();

    bool on_line(const geo2d::Line<float>& line,::cv::Point pt);
    
    void FillClusterParams(Cluster2DArray_t& cluster2d_v,const ::cv::Mat& img);

    void split_contour(const GEO2D_Contour_t& ctor,GEO2D_Contour_t& ctor1,GEO2D_Contour_t& ctor2, const geo2d::Line<float>& line);
    
    void fill_hull_and_defects(const Contour_t& ctor,
			       std::vector<int>& hullpts,
			       std::vector<cv::Vec4i>& defects,
			       std::vector<float>& defects_d);
    
    void filter_defects(std::vector<cv::Vec4i>& defects,
			std::vector<float>& defects_d,
			float min_defect_size);
    
    geo2d::Line<float> find_line_hull_defect(const Contour_t& ctor, cv::Vec4i defect);

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


