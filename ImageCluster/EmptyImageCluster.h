/**
 * \file EmptyImageCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class EmptyImageCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __EMPTYIMAGECLUSTER_H__
#define __EMPTYIMAGECLUSTER_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class EmptyImageCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class EmptyImageCluster : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    EmptyImageCluster(const std::string name="EmptyImageCluster") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~EmptyImageCluster(){}

    /// Configuration method
    void Configure(const ::fcllite::PSet &pset);

  protected:

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    std::vector<double> _canny_params;

  };
}
#endif
/** @} */ // end of doxygen group 

