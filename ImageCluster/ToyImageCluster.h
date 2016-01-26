/**
 * \file ToyImageCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ToyImageCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __TOYIMAGECLUSTER_H__
#define __TOYIMAGECLUSTER_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class ToyImageCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ToyImageCluster : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    ToyImageCluster(const std::string name="ToyImageCluster") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~ToyImageCluster(){}

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

