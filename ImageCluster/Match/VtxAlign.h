/**
 * \file VtxAlign.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class VtxAlign
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __VTXALIGN_H__
#define __VTXALIGN_H__

#include "MatchAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class VtxAlign
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class VtxAlign : public larocv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    VtxAlign(const std::string name="VtxAlign") : MatchAlgoBase(name)
    {}
    
    /// Default destructor
    ~VtxAlign(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    double _Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& roi_vtx);

    
  private:

  bool _require_3planes ; 
  float _ratio_cut ;

  };
  
  /**
     \class larocv::VtxAlignFactory
     \brief A concrete factory class for larocv::VtxAlign
   */
  class VtxAlignFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VtxAlignFactory() { AlgoFactory::get().add_factory("VtxAlign",this); }
    /// dtor
    ~VtxAlignFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new VtxAlign(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

