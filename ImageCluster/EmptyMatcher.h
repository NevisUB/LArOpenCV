/**
 * \file EmptyMatcher.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class EmptyMatcher
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __EMPTYMATCHER_H__
#define __EMPTYMATCHER_H__

#include "MatchAlgoBase.h"
#include "MatchAlgoFactory.h"

namespace larcv {
  /**
     \class EmptyMatcher
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class EmptyMatcher : public larcv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    EmptyMatcher(const std::string name="EmptyMatcher") : MatchAlgoBase(name)
    {}
    
    /// Default destructor
    ~EmptyMatcher(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    double _Process_(const larcv::Cluster2DPtrArray_t& clusters);
    
  private:

  };
  
  /**
     \class larcv::EmptyMatcherFactory
     \brief A concrete factory class for larcv::EmptyMatcher
   */
  class EmptyMatcherFactory : public MatchAlgoFactoryBase {
  public:
    /// ctor
    EmptyMatcherFactory() { MatchAlgoFactory::get().add_factory("EmptyMatcher",this); }
    /// dtor
    ~EmptyMatcherFactory() {}
    /// creation method
    MatchAlgoBase* create(const std::string instance_name) { return new EmptyMatcher(instance_name); }
  };
  /// Global larcv::EmptyMatcherFactory to register MatchAlgoFactory
  static EmptyMatcherFactory __global_EmptyMatcherFactory__;
}
#endif
/** @} */ // end of doxygen group 

