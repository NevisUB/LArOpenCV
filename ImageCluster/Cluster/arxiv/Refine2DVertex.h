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
    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

    geo2d::Line<float> SquarePCA(const ::cv::Mat& img,
				 geo2d::Vector<float> pt,
				 const size_t side);
    geo2d::VectorArray<float> QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle);
    float AngularSpread(const ::cv::Mat& polarimg,
			float radius_frac_min,
			float radius_frac_max,
			float angle_mean,
			float angle_width);
    
  private:

    void PlaneScan(const ::cv::Mat& img, const size_t plane,
		   const ::geo2d::Circle<float> init_circle);

    void XPlaneScan(const std::vector<const cv::Mat>& img_v);

    geo2d::Vector<float> MeanPixel(const cv::Mat& img, const geo2d::Vector<float>& center);

    double PointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt);

    double TwoPointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt);

    AlgorithmID_t _circle_vertex_algo_id;
    double _radius;
    float _pi_threshold;
    float _pca_box_size;
    float _global_bound_size;
    std::vector<float> _tick_offset_v;
    std::vector<float> _time_comp_factor_v;
    std::vector<float> _wire_comp_factor_v;
    bool _use_polar_spread;
    float _xplane_tick_resolution;
    float _xplane_guess_max_dist;

  };

  class Refine2DVertexData : public larocv::AlgoDataBase {
  public:
    Refine2DVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~Refine2DVertexData(){}

    void Clear() {

      _xplane_tick_min = _xplane_tick_max = -1;
      
      _valid_plane_v.clear();
      _init_vtx_v.clear();
      _scan_rect_v.clear();
      _cand_valid_v.clear();
      _cand_score_v.clear();
      _cand_vtx_v.clear();

      _valid_plane_v.resize(3,false);
      _init_vtx_v.resize(3);
      _scan_rect_v.resize(3);
      _cand_valid_v.resize(3,false);
      _cand_score_v.resize(3,-1);
      _cand_vtx_v.resize(3);
      
      _init_xs_vv.resize(3);
      _init_pca_vv.resize(3);
      _circle_trav_vv.resize(3);
      _dtheta_trav_vv.resize(3);
      _cand_xs_vv.resize(3);
      _xplane_binned_vtx_vv.resize(3);
      _xplane_binned_score_vv.resize(3);
      
      for(auto& d : _init_xs_vv)      d.clear();
      for(auto& d : _init_pca_vv)     d.clear();
      for(auto& d : _circle_trav_vv)  d.clear();
      for(auto& d : _dtheta_trav_vv)  d.clear();
      for(auto& d : _cand_xs_vv)      d.clear();

      for(auto& d : _xplane_binned_vtx_vv)   d.clear();
      for(auto& d : _xplane_binned_score_vv) d.clear();
    }

    /// bool/plane ... true = input seed existed for vertex scanning 
    std::vector< bool > _valid_plane_v;
    /// initial vertex guess (seed) [plane]
    std::vector< geo2d::Vector<float>           > _init_vtx_v;
    /// initial crossing point between the circumference and charge deposition pixel [plane][cross-point]
    std::vector< geo2d::VectorArray<float>      > _init_xs_vv;
    /// initial local PCA @ crossing point [plane][cross-point]
    std::vector< std::vector<geo2d::Line<float> > > _init_pca_vv;
    /// an array of circles that traversed to scan for the correct vtx [plane][trial]
    std::vector< std::vector< geo2d::Circle<float> > > _circle_trav_vv;
    /// an array of angle-diff-sum-over-all-xs-point [plane][trial]
    std::vector< std::vector< float >           > _dtheta_trav_vv;
    /// an array of suare box used to scan circles for vtx search
    std::vector< geo2d::Rect >  _scan_rect_v;
    /// tick range scanned for cross-plane consistency check
    float _xplane_tick_min;
    /// tick range scanned for cross-plane consistency check
    float _xplane_tick_max;
    /// "binned" (in time) best vtx estimation position per-plane, used to match across planes 
    std::vector< geo2d::VectorArray<float> > _xplane_binned_vtx_vv;
    /// "binned" (in time) best vtx estimation score per-plane, used to match across planes
    std::vector< std::vector<float > > _xplane_binned_score_vv;

    //
    // Important variables for analysis
    //

    /// the best estimate for a candidate interaction vertex per plane
    geo2d::VectorArray<float>   _cand_vtx_v;

    /*
      The scores, per plane, associated with the estimated interaction vertex.
      -1 means that it was found by a projecting a vertex found by other planes
      AND local search centered around the estimated position did not find good
      candidate. In other words, -1 shows the quality is unknown (rather not good).
      Else the value must be positive real value and the smaller the better quality.
    */
    std::vector< float >        _cand_score_v;

    /*
      Boolean flag per estimated vertex that is set if the vertex is found by
      analyzing neighboring pixels in the corresponding plane image. Similar to
      score == -1 situation if false.
    */
    std::vector< bool  >        _cand_valid_v;

    /*
      A list of points per candidate vertex that are on the circumference of
      a circle centered at the interaction vertex. It provides an estimate of
      secondary particles' trajectory point that start from the interaction
      vertex. Combining those two, particle's initial track angle can be
      obtained.
     */
    std::vector< geo2d::VectorArray<float> > _cand_xs_vv;
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

