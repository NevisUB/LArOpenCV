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
#include "AlgoData/AlgoDataVertex.h"
#include "AlgoData/Refine2DVertexData.h"


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

    float VertexTimeResolution() const { return _xplane_tick_resolution; }
    float VertexWireResolution() const { return _xplane_wire_resolution; }

    const float TimeBinMin() const { return _xplane_tick_min; }
    const float TimeBinMax() const { return _xplane_tick_max; }

    const std::vector<float>&  TimeBinnedScore0()         const { return _time_binned_score0_v;        }
    const std::vector<float>&  TimeBinnedScore0Mean()     const { return _time_binned_score0_mean_v;   }
    const std::vector<size_t>& TimeBinnedScore0MinIndex() const { return _time_binned_score0_minidx_v; }
    const std::vector<std::pair<size_t,size_t> >& TimeBinnedScore0MinRange() const { return _time_binned_score0_minrange_v; }

    const std::vector<float>&  TimeBinnedScore1()         const { return _time_binned_score1_v;        }
    const std::vector<float>&  TimeBinnedScore1Mean()     const { return _time_binned_score1_mean_v;   }
    const std::vector<size_t>& TimeBinnedScore1MinIndex() const { return _time_binned_score1_minidx_v; }
    const std::vector<std::pair<size_t,size_t> >& TimeBinnedScore1MinRange() const { return _time_binned_score1_minrange_v; }

    const std::vector<float>&  TimeBinnedScore()         const { return _time_binned_score_v;   }
    const std::vector<size_t>& TimeBinnedScoreMinIndex() const { return _time_binned_minidx_v;  }
    const std::vector<std::pair<size_t,size_t> >& TimeBinnedScoreMinRange() const { return _time_binned_minrange_v;  }

    const float WireBinMin(size_t plane) const { return _xplane_wire_min_v.at(plane); }
    const float WireBinMax(size_t plane) const { return _xplane_wire_max_v.at(plane); }
    
    const std::vector<float>& WireBinnedScore         (size_t plane) const { return _wire_binned_score_vv.at(plane); }
    const std::vector<float>& WireBinnedScoreMean     (size_t plane) const { return _wire_binned_score_mean_vv.at(plane); }
    const std::vector<size_t>& WireBinnedScoreMinIndex (size_t plane) const { return _wire_binned_score_minidx_vv.at(plane); }
    const std::vector<std::pair<size_t,size_t> >&
    WireBinnedScoreMinRange (size_t plane) const { return _wire_binned_score_minrange_vv.at(plane); }

    const std::vector<std::vector<geo2d::Vector<int> > >& VetoContour(size_t plane)
    { return _veto_ctor_vv.at(plane); }
    
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

    std::vector<geo2d::VectorArray<float> > QPointArrayOnCircleArray(const ::cv::Mat& img,
								     const geo2d::Vector<float>& pt,
								     const std::vector<float>& radius_v) const;

    float AngularSpread(const ::cv::Mat& polarimg,
			float radius_frac_min,
			float radius_frac_max,
			float angle_mean,
			float angle_width);

  private:

    bool PlaneScan(const ::cv::Mat& img, const size_t plane,
		   const geo2d::Circle<float> init_circle,
		   const geo2d::Vector<float> pt_err);

    void XPlaneTimeScan(const std::vector<const cv::Mat>& img_v);
    void XPlaneTimeProposal();
    void TimeVertex3D(const std::vector<const cv::Mat>& img_v);
    
    void XPlaneWireScan(const std::vector<const cv::Mat>& img_v);
    void XPlaneWireProposal();
    void WireVertex3D(const std::vector<const cv::Mat>& img_v);

    double PointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt);

    data::CircleVertex TwoPointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt);

    data::CircleVertex RadialScan(const cv::Mat& img, const geo2d::Vector<float>& pt);

    void ExtremePoints(const std::vector<float>& array,
		       size_t pre, size_t post, bool minimum, bool inspect_edges,
		       std::vector<size_t>& local_extreme_idx_v,
		       std::vector<std::pair<size_t,size_t> >& local_extreme_range_v,
		       float invalid_value=kINVALID_FLOAT);

    double CircleWeight(const larocv::data::CircleVertex& cvtx);

    std::vector<std::vector<geo2d::Vector<int> > > VertexVetoRegion(const ::cv::Mat& img);
      
    AlgorithmID_t _pca_algo_id;
    AlgorithmID_t _defect_algo_id;
    std::vector<float> _tick_offset_v;
    std::vector<float> _wire_comp_factor_v;
    std::vector<float> _time_comp_factor_v;
    geo2d::VectorArray<float> _origin_v;
    std::vector<std::vector<std::vector<geo2d::Vector<int> > > > _veto_ctor_vv;
    float _straight_line_angle_cut;
    float _xplane_tick_resolution;
    float _xplane_wire_resolution;
    float _xplane_guess_max_dist;
    float _vtx_3d_resolution;
    double _radius;
    double _cvtx_max_radius;
    double _cvtx_min_radius;
    double _cvtx_radius_step;
    float _pi_threshold;
    float _pca_box_size;
    float _global_bound_size;
    bool _use_polar_spread;
    float _time_exclusion_radius;
    float _wire_exclusion_radius;
    float _trigger_tick;
    std::vector< std::vector<bool> > _scan_marker_v;
    std::vector< size_t > _seed_plane_v;
    bool _require_3planes;

    float _min_contour_length;
    float _min_contour_rect_area;
    bool  _clean_image;

    // temporary data
    /// tick range scanned for cross-plane consistency check
    float _xplane_tick_min;
    /// tick range scanned for cross-plane consistency check
    float _xplane_tick_max;
    /// combined time-binned score across planes
    std::vector<float> _time_binned_score0_v;
    std::vector<float> _time_binned_score0_mean_v;
    std::vector<size_t> _time_binned_score0_minidx_v;
    std::vector<std::pair<size_t,size_t> > _time_binned_score0_minrange_v;
    /// combined time-binned score across planes
    std::vector<float> _time_binned_score1_v;
    std::vector<float> _time_binned_score1_mean_v;
    std::vector<size_t> _time_binned_score1_minidx_v;
    std::vector<std::pair<size_t,size_t> > _time_binned_score1_minrange_v;
    /// "binned" (in time) best vtx estimation score per-plane, used to match across planes
    std::vector<float> _time_binned_score_v;
    /// overall local minimum index in sampled time bins
    std::vector<size_t> _time_binned_minidx_v;
    std::vector<std::pair<size_t,size_t> > _time_binned_minrange_v;

    std::vector<float> _xplane_wire_min_v;
    std::vector<float> _xplane_wire_max_v;
    std::vector<std::vector<float> > _wire_binned_score_vv;
    std::vector<std::vector<float> > _wire_binned_score_mean_vv;
    std::vector<std::vector<size_t> > _wire_binned_score_minidx_vv;
    std::vector<std::vector<std::pair<size_t,size_t> > > _wire_binned_score_minrange_vv;

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
    data::AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new data::Refine2DVertexData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 

