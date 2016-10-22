/**
 * \file Refine2DVertex.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class Refine2DVertex
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __REFINE2DVERTEX_H__
#define __REFINE2DVERTEX_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "Core/Circle.h"
#include "Core/Line.h"
#include "Core/VectorArray.h"
namespace larocv {
  /**
     \class Refine2DVertex
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class Refine2DVertex : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    Refine2DVertex(const std::string name="Refine2DVertex") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~Refine2DVertex(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);

    geo2d::Line<float> SquarePCA(const ::cv::Mat& img,
				 geo2d::Vector<float> pt,
				 const size_t side);
    geo2d::VectorArray<float> QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle);
    
  private:

    AlgorithmID_t _circle_vertex_algo_id;
    double _radius;
    float _pi_threshold;
    float _pca_box_size;
    float _global_bound_size;
  };

  class Refine2DVertexData : public larocv::AlgoDataBase {
  public:
    Refine2DVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~Refine2DVertexData(){}

    void Clear() {
      _vtx_v.resize(3);
      _xs_vv.resize(3);
      _pca_vv.resize(3);
      _cand_vtx_vv.resize(3);
      _cand_score_vv.resize(3);
      _circle_trav_vv.resize(3);
      
      for(auto& d : _xs_vv)         d.clear();
      for(auto& d : _pca_vv)        d.clear();
      for(auto& d : _cand_score_vv) d.clear();
      for(auto& d : _cand_vtx_vv)   d.clear();
      for(auto& d : _circle_trav_vv)   d.clear();

      
    }

    std::vector< geo2d::Vector<float>           > _vtx_v;
    std::vector< geo2d::VectorArray<float>      > _xs_vv;
    std::vector< std::vector<geo2d::Line<float> > > _pca_vv;
    std::vector< std::vector<double >           > _cand_score_vv;
    std::vector< geo2d::VectorArray<float>      > _cand_vtx_vv;
    std::vector< std::vector< geo2d::Circle<float> > > _circle_trav_vv;
  };
  
  /**
     \class larocv::Refine2DVertexFactory
     \brief A concrete factory class for larocv::Refine2DVertex
   */
  class Refine2DVertexFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    Refine2DVertexFactory() { ClusterAlgoFactory::get().add_factory("Refine2DVertex",this); }
    /// dtor
    ~Refine2DVertexFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new Refine2DVertex(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new Refine2DVertexData(instance_name,id);}
  };
  /// Global larocv::Refine2DVertexFactory to register ClusterAlgoFactory
  static Refine2DVertexFactory __global_Refine2DVertexFactory__;
}
#endif
/** @} */ // end of doxygen group 

