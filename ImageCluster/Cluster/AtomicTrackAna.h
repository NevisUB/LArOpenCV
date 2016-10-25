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
    void _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    AlgorithmID_t _pca_algo_id;
  };

  class AtomicTrackAnaData : public larocv::AlgoDataBase {
  public:
    AtomicTrackAnaData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~AtomicTrackAnaData(){}

    void Clear() {}

  };
  
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
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new AtomicTrackAnaData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 

