#ifndef __VERTEXSCANNER3D_H__
#define __VERTEXSCANNER3D_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexScan3D.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
/*
  @brief: determine 3D shower vertex seeds from 3D edge points, 3D track vertex points, and 3D end points
*/
namespace larocv {

  class VertexScanner3D : public larocv::ImageAnaBase {

  public:

    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexScanner3D(const std::string name = "VertexScanner3D") :
      ImageAnaBase(name),
      _VertexScan3D()
    {
      _vertex3d_id = kINVALID_ALGO_ID;
      _super_cluster_id = kINVALID_ALGO_ID;
      _linear_track_id = kINVALID_ALGO_ID;
      _compound_id = kINVALID_ALGO_ID;
    }

    /// Default destructor
    virtual ~VertexScanner3D() {}

    /// Finalize after process
    void Finalize(TFile*) {}

    void Reset() { _seed_cands_v.clear(); _vtx3d_cands_v.clear(); }

    const std::vector<larocv::data::Vertex3D>& VertexArray() const { return _vtx3d_cands_v; }

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);

    void _Process_(const Cluster2DArray_t& clusters,
                   ::cv::Mat& img,
                   ImageMeta& meta,
                   ROI& roi)
    { _VertexScan3D.SetPlaneInfo(meta); }

    bool _PostProcess_(std::vector<cv::Mat>& img_v);

    void RegisterSeed(const larocv::data::Vertex3D& vtx3d,
                      const data::SeedType_t type,
                      std::vector<cv::Mat> img_v);

    void RegisterSeed(const std::vector<data::VertexTrackInfoCollection>& vtxinfo_v,
                      std::vector<cv::Mat> img_v);

  private:

    AlgorithmID_t _vertex3d_id;
    AlgorithmID_t _super_cluster_id;
    AlgorithmID_t _linear_track_id;
    AlgorithmID_t _compound_id;
    std::vector<larocv::data::VertexSeed3D> _seed_cands_v;
    std::vector<larocv::data::Vertex3D> _vtx3d_cands_v;
    VertexScan3D  _VertexScan3D;
  };

  /**
     \class VertexScanner3DFactory
     \brief A concrete factory class for VertexScanner3D
  */
  class VertexScanner3DFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexScanner3DFactory() { AlgoFactory::get().add_factory("VertexScanner3D", this); }
    /// dtor
    ~VertexScanner3DFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexScanner3D(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group
