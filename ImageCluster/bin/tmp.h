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

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class ClassName
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ClassName : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    ClassName(const std::string name="ClassName") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~ClassName(){}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    std::vector<double> _canny_params;

  };

  class ClassNameFactory : public ImageClusterFactoryBase {
  public:
    ClassNameFactory() { ImageClusterFactory::get().add_factory("ClassName",this); }
    ~ClassNameFactory() {}
    ImageClusterBase* create() { return new ClassName; }
  };

  static ClassNameFactory __global_ClassNameFactory__;
}
#endif
/** @} */ // end of doxygen group 

