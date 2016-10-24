//by vic

#ifndef __DEFECTCLUSTER_H__
#define __DEFECTCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

#include "Core/Geo2D.h"

#include "DefectClusterData.h"


namespace larocv {
 
  class DefectCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    DefectCluster(const std::string name = "DefectCluster") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~DefectCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi);


    cv::Vec4i max_hull_edge(const GEO2D_Contour_t& ctor, std::vector<cv::Vec4i> defects);

    bool on_line(const geo2d::Line<float>& line,::cv::Point pt);
    
    void split_contour(const GEO2D_Contour_t& ctor,GEO2D_Contour_t& ctor1,GEO2D_Contour_t& ctor2, const geo2d::Line<float>& line);
    
    void fill_hull_and_defects(const Contour_t& ctor,
			       std::vector<int>& hullpts,
			       std::vector<cv::Vec4i>& defects,
			       std::vector<float>& defects_d);
    
    void filter_defects(std::vector<cv::Vec4i>& defects,
			std::vector<float>& defects_d,
			float min_defect_size);
    
    geo2d::Line<float> find_line_hull_defect(const Contour_t& ctor, cv::Vec4i defect);

    geo2d::Line<float> scan_breaker(const Contour_t& ctor, cv::Vec4i defect);
    
  private:
    int _min_defect_size;
    int _hull_edge_pts_split;

  };

  /**
     \class larocv::DefectClusterFactory
     \brief A concrete factory class for larocv::DefectCluster
  */
  class DefectClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    DefectClusterFactory() { AlgoFactory::get().add_factory("DefectCluster",this); }
    /// dtor
    ~DefectClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new DefectCluster(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new DefectClusterData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

