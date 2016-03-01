/**
 * \file ClassName.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ClassName
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CLASSNAME_H__
#define __CLASSNAME_H__

#include "MatchAlgoBase.h"

namespace larcv {
  /**
     \class ClassName
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class ClassName : public larcv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    ClassName(const std::string name="ClassName") : MatchAlgoBase(name)
    {}
    
    /// Default destructor
    ~ClassName(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    double _Process_(const larcv::Cluster2D_t& clusters,
		     larcv::ImageMeta& meta);
    
  private:

  };
  
  /**
     \class larcv::ClassNameFactory
     \brief A concrete factory class for larcv::ClassName
   */
  class ClassNameFactory : public MatchAlgoFactoryBase {
  public:
    /// ctor
    ClassNameFactory() { MatchAlgoFactory::get().add_factory("ClassName",this); }
    /// dtor
    ~ClassNameFactory() {}
    /// creation method
    MatchAlgoBase* create(const std::string instance_name) { return new ClassName(instance_name); }
  };
  /// Global larcv::ClassNameFactory to register MatchAlgoFactory
  static ClassNameFactory __global_ClassNameFactory__;
}
#endif
/** @} */ // end of doxygen group 

