/**
 * \file SmoothBinaryCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SmoothBinaryCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SMOOTHBINARYCLUSTER_H__
#define __SMOOTHBINARYCLUSTER_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class SmoothBinaryCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SmoothBinaryCluster : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    SmoothBinaryCluster(const std::string name="SmoothBinaryCluster") :
      ImageClusterBase(name),
      _dilation_size ( 5 ),
      _dilation_iter ( 2 ),
      _blur_size     ( 5 ),
      _thresh        ( 1 ),
      _thresh_maxval (255)
    {}
    
    /// Default destructor
    ~SmoothBinaryCluster(){}

    /// Configuration method
    void Configure(const ::fcllite::PSet &pset);

  protected:

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:
    
    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    float _thresh;
    float _thresh_maxval;

    //float _polygon_e;
    
  };
}
#endif
/** @} */ // end of doxygen group 

