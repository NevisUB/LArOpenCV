#ifndef __VERTEXSINGLESHOWER_H__
#define __VERTEXSINGLESHOWER_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "AlgoData/SingleShowerData.h"
#include "AlgoData/dQdXProfilerData.h"
#include "AlgoData/VertexClusterData.h"
#include "AlgoData/LinearTrackClusterData.h"
#include "AlgoClass/LArPlaneGeo.h"
namespace larocv {
 
  class VertexSingleShower : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexSingleShower(const std::string name = "VertexSingleShower") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~VertexSingleShower(){}

    /// Finalize after process
    void Finalize(TFile*) {}

    const std::vector<larocv::data::Vertex3D>& ScannedVertex3D() const
    { return _shower_vtx3d_v; }
    
    const std::vector<std::vector<larocv::data::CircleVertex> >& ScannedVertex2D() const
    { return _shower_vtx2d_vv; }

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    void Reset();
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);
    
  private:

    void RetrieveVertex(const std::vector<const cv::Mat>& img_v,
			const larocv::data::VertexClusterArray& part,
			const larocv::data::dQdXProfilerData& dqdx);
    void RetrieveVertex(const larocv::data::LinearTrackArray& data);
    std::vector<larocv::data::Vertex3D>
    ListCandidateVertex(const std::vector<larocv::data::Vertex3D>& ltrack_vertex_v,
			const std::vector<larocv::data::Vertex3D>& vtrack_vertex_v,
			const std::vector<larocv::data::Vertex3D>& vedge_vertex_v) const;
    larocv::data::ShowerCluster
    SingleShowerHypothesis(const ::cv::Mat& img,
			   const size_t plane,
			   const data::CircleVertex& vtx);
    geo2d::VectorArray<float>
    ValidShowerPointOnCircle(const ::cv::Mat& img,
			     const geo2d::Circle<float>& circle,
			     const std::vector<geo2d::Vector<float> >& xs_pts) const;
    
    void ListShowerVertex(const std::vector<const cv::Mat>& img_v,
			  const std::vector<larocv::data::Vertex3D>& cand_v,
			  std::vector<larocv::data::Vertex3D>& res_vtx_v,
			  std::vector<std::vector<larocv::data::CircleVertex> >& res_cvtx_v) const;

    LArPlaneGeo _geo_algo;
    std::vector<larocv::data::Vertex3D> _cand_vertex_v;
    AlgorithmID_t _algo_id_dqdx;
    AlgorithmID_t _algo_id_vertex_track;
    AlgorithmID_t _algo_id_linear_track;
    float _part_pxfrac_threshold;
    float _theta_lo;
    float _theta_hi;

    float _vertex_min_separation;
    float _circle_default_radius;
    float _circle_min_separation;
    size_t _valid_xs_npx;
    size_t _num_planes;
    float  _pi_threshold;
    float  _min_compat_dist;
    float _xplane_tick_resolution;
    std::vector<size_t> _seed_plane_v;

    std::vector<larocv::data::Vertex3D> _ltrack_vertex_v;
    std::vector<larocv::data::Vertex3D> _vtrack_vertex_v;
    std::vector<larocv::data::Vertex3D> _vedge_vertex_v;

    std::vector<larocv::data::Vertex3D> _shower_vtx3d_v;
    std::vector<std::vector<larocv::data::CircleVertex> > _shower_vtx2d_vv;
    
  };

  /**
     \class larocv::VertexSingleShowerFactory
     \brief A concrete factory class for larocv::VertexSingleShower
   */
  class VertexSingleShowerFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexSingleShowerFactory() { AlgoFactory::get().add_factory("VertexSingleShower",this); }
    /// dtor
    ~VertexSingleShowerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexSingleShower(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

