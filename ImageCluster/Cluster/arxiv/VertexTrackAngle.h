/**
 * \file VertexTrackAngle.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class VertexTrackAngle
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __VERTEXTRACKANGLE_H__
#define __VERTEXTRACLANGLE_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class VertexTrackAngle
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class VertexTrackAngle : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor
    VertexTrackAngle(const std::string name="VertexTrackAngle") : ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~VertexTrackAngle(){}

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
    AlgorithmID_t _vtx_algo_id;
  };

  class VertexTrackAngleData : public larocv::AlgoDataBase {
  public:
    VertexTrackAngleData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~VertexTrackAngleData(){}

    void Clear() { _nplanes = 0; }

    size_t _nplanes;
    
  };
  
  /**
     \class larocv::VertexTrackAngleFactory
     \brief A concrete factory class for larocv::VertexTrackAngle
   */
  class VertexTrackAngleFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexTrackAngleFactory() { AlgoFactory::get().add_factory("VertexTrackAngle",this); }
    /// dtor
    ~VertexTrackAngleFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new VertexTrackAngle(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new VertexTrackAngleData(instance_name,id);}
  };
}
#endif
/** @} */ // end of doxygen group 

