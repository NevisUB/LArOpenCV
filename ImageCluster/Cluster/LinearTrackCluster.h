#ifndef __LINEARTRACKCLUSTER_H__
#define __LINEARTRACKCLUSTER_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "AlgoData/LinearTrackClusterData.h"

namespace larocv {
 
  class LinearTrackCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LinearTrackCluster(const std::string name = "LinearTrackCluster") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~LinearTrackCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    void FindEdges(const cv::Mat& img,
		   const GEO2D_Contour_t& ctor,
		   geo2d::Vector<float>& edge1,
		   geo2d::Vector<float>& edge2) const;

    void EdgesFromMeanValue(const GEO2D_Contour_t& ctor,
			    geo2d::Vector<float>& edge1,
			    geo2d::Vector<float>& edge2) const;
    
    void EdgesFromPCAProjection(const cv::Mat& img,
				const GEO2D_Contour_t& ctor,
				geo2d::Vector<float>& edge1,
				geo2d::Vector<float>& edge2) const;
    
    std::vector<larocv::data::LinearTrack2D> FindLinearTrack2D(size_t plane, const cv::Mat& img) const;

    void FindLinearTrack(const std::vector<std::vector<larocv::data::LinearTrack2D> >& strack_vv);

    geo2d::Line<float> calc_pca(const GEO2D_Contour_t & ctor) const;
    
    float y2wire(float y, const size_t plane) const;
    float x2tick(float x, const size_t plane) const;
    float wire2y(float wire, const size_t plane) const;
    float tick2x(float tick, const size_t plane) const;
    bool  YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
		  const geo2d::Vector<float>& pt1, const size_t plane1,
		  larocv::data::Vertex3D& result) const;

    bool _edges_from_mean;
    float _minimum_neighbor_distance;
    size_t _num_planes;
    AlgorithmID_t _algo_id_part;
    float  _part_pxfrac_threshold;
    size_t _min_size_track_ctor;
    float  _min_length_track_ctor;
    float  _pi_threshold;
    float  _min_compat_dist;
    float _xplane_tick_resolution;
    std::vector<size_t> _seed_plane_v;
    float  _trigger_tick;
    std::vector<float> _tick_offset_v;
    std::vector<float> _wire_comp_factor_v;
    std::vector<float> _time_comp_factor_v;
    geo2d::VectorArray<float> _origin_v;
  };

  /**
     \class larocv::LinearTrackClusterFactory
     \brief A concrete factory class for larocv::LinearTrackCluster
   */
  class LinearTrackClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    LinearTrackClusterFactory() { AlgoFactory::get().add_factory("LinearTrackCluster",this); }
    /// dtor
    ~LinearTrackClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new LinearTrackCluster(instance_name); }
    /// data create method
    data::AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new data::LinearTrackArray(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

