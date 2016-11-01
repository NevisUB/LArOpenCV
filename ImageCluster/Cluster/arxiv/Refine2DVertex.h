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

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "Core/Circle.h"
#include "Core/Line.h"
#include "Core/VectorArray.h"
#include "Core/BoundingBox.h"
namespace larocv {
  /**
     \class Refine2DVertex
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class Refine2DVertex : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor
    Refine2DVertex(const std::string name="Refine2DVertex") : ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~Refine2DVertex(){}

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
    void _PostProcess_();

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
    std::vector<float> _tick_offset_v;
    std::vector<float> _comp_factor_v;
  };

  class Refine2DVertexData : public larocv::AlgoDataBase {
  public:
    Refine2DVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~Refine2DVertexData(){}

    void Clear() {
      
      _vtx_v.clear();
      _scan_rect_v.clear();
      _cand_score_v.clear();
      _cand_vtx_v.clear();

      _vtx_v.resize(3);
      _scan_rect_v.resize(3);
      _cand_score_v.resize(3,-1);
      _cand_vtx_v.resize(3);
      
      _xs_vv.resize(3);
      _pca_vv.resize(3);
      _circle_trav_vv.resize(3);
      _dtheta_trav_vv.resize(3);
      
      for(auto& d : _xs_vv)         d.clear();
      for(auto& d : _pca_vv)        d.clear();
      for(auto& d : _circle_trav_vv)  d.clear();
      for(auto& d : _dtheta_trav_vv)  d.clear();
      
    }

    std::vector< geo2d::Vector<float>           > _vtx_v;
    std::vector< geo2d::VectorArray<float>      > _xs_vv;
    std::vector< std::vector<geo2d::Line<float> > > _pca_vv;
    std::vector< std::vector< geo2d::Circle<float> > > _circle_trav_vv;
    std::vector< std::vector< float >           > _dtheta_trav_vv;
    std::vector< geo2d::Rect >  _scan_rect_v;
    std::vector< float >        _cand_score_v;
    geo2d::VectorArray<float>   _cand_vtx_v;
  };
  
  /**
     \class larocv::Refine2DVertexFactory
     \brief A concrete factory class for larocv::Refine2DVertex
   */
  class Refine2DVertexFactory : public AlgoFactoryBase {
  public:
    /// ctor
    Refine2DVertexFactory() { AlgoFactory::get().add_factory("Refine2DVertex",this); }
    /// dtor
    ~Refine2DVertexFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new Refine2DVertex(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new Refine2DVertexData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 
