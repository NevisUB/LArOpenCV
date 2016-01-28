/**
 * \file TCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __TCLUSTER_H__
#define __TCLUSTER_H__

#include "ImageClusterBase.h"
#include "ImageClusterViewer.h"
#include "FillImageClusterVariables.h"

namespace larcv {
  /**
     \class TCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class TCluster : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    TCluster(const std::string name="TCluster") : 
       ImageClusterBase(name),
       _blur_x(3),
       _blur_y(3),
       _thresh(1),
       _threshMaxVal(255),
       _dilation_size(5),
       _dilation_iter(2)
    {}
    
    /// Default destructor
    ~TCluster(){}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    larcv::FillImageClusterVariables _filler ;

    int _blur_x;
    int _blur_y;
    int _thresh;
    int _threshMaxVal;
    int _dilation_size;
    int _dilation_iter; 

  };

  class TClusterFactory : public ImageClusterFactoryBase {
  public:
    TClusterFactory() { ImageClusterFactory::get().add_factory("TCluster",this); }
    ~TClusterFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new TCluster(instance_name); }
  };

  static TClusterFactory __global_TClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

