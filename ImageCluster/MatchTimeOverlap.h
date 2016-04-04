/**
 * \file MatchTimeOverlap.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MatchTimeOverlap
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __MATCHTIMEOVERLAP_H__
#define __MATCHTIMEOVERLAP_H__

#include "MatchAlgoBase.h"
#include "MatchAlgoFactory.h"

namespace larcv {
  /**
     \class MatchTimeOverlap
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class MatchTimeOverlap : public larcv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    MatchTimeOverlap(const std::string name="MatchTimeOverlap") : MatchAlgoBase(name)
    {}
    
    /// Default destructor
    ~MatchTimeOverlap(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    double _Process_(const larcv::Cluster2DPtrArray_t& clusters);
		     
    
  private:
    float _time_ratio_cut ;
    float _start_time_cut ;
    bool _require_3planes;

    double _pixel_y(const Cluster2D* cluster,size_t pix);
    
  };
  
  /**
     \class larcv::MatchTimeOverlapFactory
     \brief A concrete factory class for larcv::MatchTimeOverlap
   */
  class MatchTimeOverlapFactory : public MatchAlgoFactoryBase {
  public:
    /// ctor
    MatchTimeOverlapFactory() { MatchAlgoFactory::get().add_factory("MatchTimeOverlap",this); }
    /// dtor
    ~MatchTimeOverlapFactory() {}
    /// creation method
    MatchAlgoBase* create(const std::string instance_name) { return new MatchTimeOverlap(instance_name); }
  };
  /// Global larcv::MatchTimeOverlapFactory to register MatchAlgoFactory
  static MatchTimeOverlapFactory __global_MatchTimeOverlapFactory__;
}
#endif
/** @} */ // end of doxygen group 

