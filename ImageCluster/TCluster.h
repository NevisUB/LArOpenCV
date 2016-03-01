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
#include "TClusterVars.h"

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
       _size_x(150),
       _size_y(150)
    {}
    
    /// Default destructor
    ~TCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    int _blur_x;
    int _blur_y;
    float _thresh;
    float _threshMaxVal;
    float _size_x ;
    float _size_y ;

  };

  class TClusterFactory : public ImageClusterFactoryBase {
  public:
    TClusterFactory() { ImageClusterFactory::get().add_factory("TCluster",this); }
    ~TClusterFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new TCluster(instance_name); }
    /// parameter creator
    AlgoVarsBase* vars() { return new TClusterVars; }
  };

  static TClusterFactory __global_TClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

