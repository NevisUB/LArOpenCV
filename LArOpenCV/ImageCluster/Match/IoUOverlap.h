/**
 * \file IoUOverlap.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class IoUOverlap
 *
 * @author david c
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __IOUOVERLAP_H__
#define __IOUOVERLAP_H__

#include "LArOpenCV/ImageCluster/Base/MatchAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class IoUOverlap
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class IoUOverlap : public larocv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    IoUOverlap(const std::string name="IoUOverlap") : MatchAlgoBase(name)
    {}
    
    /// Default destructor
    ~IoUOverlap(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    double _Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& roi_vtx); 
		     
    
  private:

    double _pixel_y(const Cluster2D* cluster,size_t pix);
    void getMinMaxTime(const Cluster2D* cluster, double& min, double& max);
  };
  
  /**
     \class larocv::IoUOverlapFactory
     \brief A concrete factory class for larocv::IoUOverlap
   */
  class IoUOverlapFactory : public AlgoFactoryBase {
  public:
    /// ctor
    IoUOverlapFactory() { AlgoFactory::get().add_factory("IoUOverlap",this); }
    /// dtor
    ~IoUOverlapFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new IoUOverlap(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

