/**
 * \file AtomicTrackAna.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class AtomicTrackAna
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __ATOMICTRACKANA_H__
#define __ATOMICTRACKANA_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "Core/VectorArray.h"

namespace larocv {
  /**
     \class AtomicTrackAna
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class AtomicTrackAna : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor
    AtomicTrackAna(const std::string name="AtomicTrackAna") : ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~AtomicTrackAna(){}

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

  private:

    AlgorithmID_t _pca_algo_id;
    AlgorithmID_t _def_algo_id;
    AlgorithmID_t _ref_algo_id;

  };

  namespace data {
    class AtomicTrackAnaData : public AlgoDataBase {
    public:
      AtomicTrackAnaData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      { Clear();}
      ~AtomicTrackAnaData(){}

      void Clear() {
	_atomic_idx_per_clu_v_v_v.resize(3);
	_atomic_spread_vvv.resize(3);
	_pts_vv.resize(3);
	for (unsigned i=0;i<3;++i){
	  _atomic_idx_per_clu_v_v_v[i].clear();
	  _atomic_spread_vvv[i].clear();
	  _pts_vv[i].clear();
	}
      
      }
    
      //Plane//Track//Atomics
      std::vector<std::vector<GEO2D_ContourArray_t>>   _atomic_order_ctor_v_v;
      //Plane//Track//Atomic Kink
      std::vector<std::vector<std::vector<cv::Point>>> _atomic_kink_vv;
      //Plane//Track//Atomic Spread
      std::vector<std::vector<std::vector<double> > >  _atomic_spread_vvv;
      //Plane//Track//Atomic Index
      std::vector<std::vector<std::vector<int>>>       _atomic_idx_per_clu_v_v_v;
      //Plane//Points
      std::vector<geo2d::VectorArray<int>>             _pts_vv;

      //cv::Mat _img;
      //std::vector<std::vector<float>>     _atomic_track_spread_v_v;//Each cluser has an average value of spread form pixels to PCA line
      //std::vector<std::vector<size_t>>    _atomic_track_ass_v_v;   //Association Number between cluster and linesegmen
      //plane//atomic
      //std::vector<std::vector<std::vector<cv::Point>>> _atomic_track_order_v_v_v;
      //plane //original cluster
    
    
    };
  }  
  /**
     \class larocv::AtomicTrackAnaFactory
     \brief A concrete factory class for larocv::AtomicTrackAna
   */
  class AtomicTrackAnaFactory : public AlgoFactoryBase {
  public:
    /// ctor
    AtomicTrackAnaFactory() { AlgoFactory::get().add_factory("AtomicTrackAna",this); }
    /// dtor
    ~AtomicTrackAnaFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new AtomicTrackAna(instance_name); }
    /// data create method
    data::AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new data::AtomicTrackAnaData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 

