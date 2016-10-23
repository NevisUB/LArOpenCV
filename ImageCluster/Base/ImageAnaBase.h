/**
 * \file ImageAnaBase.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ImageAnaBase
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __IMAGEANABASE_H__
#define __IMAGEANABASE_H__

#include "ImageClusterBase.h"
#include "Cluster2D.h"

namespace larocv {

  /**
     \class ImageAnaBase
     DESCRIBE ME
  */
  class ImageAnaBase : public ImageClusterBase {

  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ImageAnaBase(const std::string name = "noname");
    
    /// Default destructor
    virtual ~ImageAnaBase(){}

    /// Configuration method
    void Configure(const ::fcllite::PSet &pset);

    /// Algorithm Type
    AlgorithmType_t Type() const { return kAlgoImageAna; }

    /**
       @brief wrapper execution method: internally executes _Process_ function (see there for details)
    */
    void Process(const larocv::Cluster2DArray_t& clusters,
		 const ::cv::Mat& img,
		 larocv::ImageMeta& meta,
		 larocv::ROI & roi);

    /**
       @brief wrapper execution method: internally executes _PostProcess_ function (see there for details)
     */
    void PostProcess();

  protected:

    /// Inherited class configuration method
    virtual void _Configure_(const ::fcllite::PSet &pset) = 0;

    /**
       DESCRIBE ME
     */
    virtual void _Process_(const larocv::Cluster2DArray_t& clusters,
			   const ::cv::Mat& img,
			   larocv::ImageMeta& meta,
			   larocv::ROI& roi) = 0;
    /**
       DESCRIBE ME
     */
    virtual void _PostProcess_() {}

  };
  
}
#endif
/** @} */ // end of doxygen group 

