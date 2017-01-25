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
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class EmptyMatcher
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class EmptyMatcher : public larocv::MatchAlgoBase {
    
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
    double _Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& vtx); 
    
  private:

  };
  
  /**
     \class larocv::EmptyMatcherFactory
     \brief A concrete factory class for larocv::EmptyMatcher
   */
  class EmptyMatcherFactory : public AlgoFactoryBase {
  public:
    /// ctor
    EmptyMatcherFactory() { AlgoFactory::get().add_factory("EmptyMatcher",this); }
    /// dtor
    ~EmptyMatcherFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new EmptyMatcher(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

