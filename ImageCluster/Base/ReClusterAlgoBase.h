/**
 * \file ReClusterAlgoBase.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ReClusterAlgoBase
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __RECLUSTERALGOBASE_H__
#define __RECLUSTERALGOBASE_H__

#include "ImageClusterBase.h"
#include "Cluster2D.h"

namespace larocv {

  /**
     \class ReClusterAlgoBase
  */
  class ReClusterAlgoBase : public ImageClusterBase {

  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ReClusterAlgoBase(const std::string name = "noname");
    
    /// Default destructor
    virtual ~ReClusterAlgoBase(){}

    /// Configuration method
    void Configure(const ::fcllite::PSet &pset);

    /// Reset method
    virtual void Reset(){}

    /// Algorithm Type
    AlgorithmType_t Type() const { return kAlgoReCluster;}

    /**
       @brief wrapper execution method: internally executes _Process_ function (see there for details)
    */
    larocv::Cluster2DArray_t Process(const larocv::Cluster2DArray_t& clusters,
				    const ::cv::Mat& img,
				    larocv::ImageMeta& meta);

  protected:

    /// Inherited class configuration method
    virtual void _Configure_(const ::fcllite::PSet &pset) = 0;

    /**
       TBD
     */
    virtual larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta) = 0;
    
  };

}
#endif
/** @} */ // end of doxygen group 

