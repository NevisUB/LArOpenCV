/**
 * \file MatchCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MatchCluster
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __MATCHCLUSTER_H__
#define __MATCHCLUSTER_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "MatchBookKeeper.h"

namespace larocv {
  /**
     \class MatchCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class MatchCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor
    MatchCluster(const std::string name="MatchCluster") : ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~MatchCluster(){}

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
    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

    float dist(const ::cv::Point& pt1, const Point2D& pt2);

    void getMinMaxTime(const Cluster2D* cluster, double& min, double& max);

    double _pixel_y(const Cluster2D* cluster,size_t pix);

    const MatchBookKeeper& BookKeeper() const { return _book_keeper; }


  private:

    AlgorithmID_t _vtx_algo_id;
    AlgorithmID_t _contour_algo_id;
    MatchBookKeeper _book_keeper;

  };

  class MatchClusterData : public larocv::AlgoDataBase {
  public:
    MatchClusterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~MatchClusterData(){}

    void Clear() { 
      _nplanes = 0; 
      _endpts_v_v.clear();
      _endpts_v_v.resize(3); 
      
      _clusters.clear();
      _clusters.resize(3);
       }

    size_t _nplanes;
    std::vector<std::vector<std::pair<float,float>>> _endpts_v_v;

    // A vector of matches (assume only 2 matches for now
    std::vector<std::vector<float>> _matches ;
    std::vector<larocv::Cluster2DArray_t> _clusters ;
    
  };
  
  /**
     \class larocv::MatchClusterFactory
     \brief A concrete factory class for larocv::MatchCluster
   */
  class MatchClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    MatchClusterFactory() { AlgoFactory::get().add_factory("MatchCluster",this); }
    /// dtor
    ~MatchClusterFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new MatchCluster(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new MatchClusterData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 

