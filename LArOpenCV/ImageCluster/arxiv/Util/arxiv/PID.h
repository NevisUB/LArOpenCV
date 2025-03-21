/**
 * \file PID.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PID
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PID_H__
#define __PID_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class PID
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PID : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PID(const std::string name="PID") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PID(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:

  int _pdg;

  };
  
  /**
     \class larocv::PIDFactory
     \brief A concrete factory class for larocv::PID
   */
  class PIDFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PIDFactory() { AlgoFactory::get().add_factory("PID",this); }
    /// dtor
    ~PIDFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PID(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

