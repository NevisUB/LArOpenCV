/**
 * \file ChargeDistribOverlap.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ChargeDistribOverlap
 *
 * @author DC
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHARGEDISTRIBOVERLAP_H__
#define __CHARGEDISTRIBOVERLAP_H__

#include "MatchAlgoBase.h"
#include "MatchAlgoFactory.h"
#include "LArUtil/PxUtils.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/DetectorProperties.h"
namespace larocv {
  /**
     \class ChargeDistribOverlap
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class ChargeDistribOverlap : public larocv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    ChargeDistribOverlap(const std::string name="ChargeDistribOverlap") : MatchAlgoBase(name), _verbose(false)
    {}
    
    /// Default destructor
    ~ChargeDistribOverlap(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    double _Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& roi_vtx); 
		     
    
  private:

    float TProfConvol(std::vector<::larutil::PxHit> hA ,std::vector<::larutil::PxHit> hB);
    
    double _pixel_y(const Cluster2D* cluster,size_t pix);
    double _pixel_x(const Cluster2D* cluster,size_t pix);

    bool _verbose;
    bool _debug;
    
  };
  
  /**
     \class larocv::ChargeDistribOverlapFactory
     \brief A concrete factory class for larocv::ChargeDistribOverlap
   */
  class ChargeDistribOverlapFactory : public MatchAlgoFactoryBase {
  public:
    /// ctor
    ChargeDistribOverlapFactory() { MatchAlgoFactory::get().add_factory("ChargeDistribOverlap",this); }
    /// dtor
    ~ChargeDistribOverlapFactory() {}
    /// creation method
    MatchAlgoBase* create(const std::string instance_name) { return new ChargeDistribOverlap(instance_name); }
  };
  /// Global larocv::ChargeDistribOverlapFactory to register MatchAlgoFactory
  static ChargeDistribOverlapFactory __global_ChargeDistribOverlapFactory__;
}
#endif
/** @} */ // end of doxygen group 

