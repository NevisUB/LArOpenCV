#ifndef __DQDXPROFILER_H__
#define __DQDXPROFILER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/ImageCluster/AlgoData/dQdXProfilerData.h"
#include "LArOpenCV/ImageCluster/AlgoData/PCACandidatesData.h"
#include "LArOpenCV/ImageCluster/AlgoData/DefectClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoData/Refine2DVertexData.h"

namespace larocv {
 
  class dQdXProfiler : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    dQdXProfiler(const std::string name = "dQdXProfiler") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~dQdXProfiler(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    double DistanceAtom2Vertex(const data::AtomicContour& atom, const geo2d::Vector<float>& vtx2d) const;
    double DistanceAtom2Vertex(const data::AtomicContour& atom, const data::CircleVertex&   vtx2d) const;
      
    std::vector<size_t> OrderAtoms(const data::ClusterCompound& cluster, const geo2d::Vector<float>& vtx2d) const;
    std::vector<size_t> OrderAtoms(const data::ClusterCompound& cluster, const data::CircleVertex&   vtx2d) const;

    std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
    AtomsEdge(const data::ClusterCompound& cluster, const geo2d::Vector<float>& vtx2d, const std::vector<size_t> atom_order_v) const;
    
    std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
    AtomsEdge(const data::ClusterCompound& cluster, const data::CircleVertex& vtx2d, const std::vector<size_t> atom_order_v) const;

    std::vector<float> AtomdQdX(const cv::Mat& img, const data::AtomicContour& atom,
				const geo2d::Line<float>& pca,
				const geo2d::Vector<float>& start,
				const geo2d::Vector<float>& end) const;
    
    AlgorithmID_t _pca_algo_id;
    AlgorithmID_t _vtxtrack_algo_id;
    AlgorithmID_t _lintrack_algo_id;
    
    float _pi_threshold;
    float _dx_resolution;
    float _atomic_region_pad;
    float _atomic_contour_pad;
  };

  /**
     \class larocv::dQdXProfilerFactory
     \brief A concrete factory class for larocv::dQdXProfiler
   */
  class dQdXProfilerFactory : public AlgoFactoryBase {
  public:
    /// ctor
    dQdXProfilerFactory() { AlgoFactory::get().add_factory("dQdXProfiler",this); }
    /// dtor
    ~dQdXProfilerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new dQdXProfiler(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

