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

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class ClassName
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ClassName : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    ClassName(const std::string name="ClassName") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~ClassName(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);
    
  private:

  };
  
  /**
     \class larcv::ClassNameFactory
     \brief A concrete factory class for larcv::ClassName
   */
  class ClassNameFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    ClassNameFactory() { ClusterAlgoFactory::get().add_factory("ClassName",this); }
    /// dtor
    ~ClassNameFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new ClassName(instance_name); }
  };
  /// Global larcv::ClassNameFactory to register ClusterAlgoFactory
  static ClassNameFactory __global_ClassNameFactory__;
}
#endif
/** @} */ // end of doxygen group 

