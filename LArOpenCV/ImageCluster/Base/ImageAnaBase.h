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
    void Configure(const Config_t &pset);

    /// Reset method
    virtual void Reset(){}

    /// Algorithm Type
    AlgorithmType_t Type() const { return kAlgoImageAna; }

    /**
       @brief wrapper execution method: internally executes _Process_ function (see there for details)
    */
    void Process(const larocv::Cluster2DArray_t& clusters,
		 ::cv::Mat& img,
		 larocv::ImageMeta& meta,
		 larocv::ROI & roi);

    /**
       @brief wrapper execution method: internally executes _PostProcess_ function (see there for details)
     */
    bool PostProcess(std::vector<cv::Mat>& img_v);

  protected:

    /// Inherited class configuration method
    virtual void _Configure_(const Config_t &pset) = 0;

    /**
       DESCRIBE ME
     */
    virtual void _Process_(const larocv::Cluster2DArray_t& clusters,
			   ::cv::Mat& img,
			   larocv::ImageMeta& meta,
			   larocv::ROI& roi) = 0;
    /**
       DESCRIBE ME
     */
    virtual bool _PostProcess_(std::vector<cv::Mat>& img_v) = 0;

  };
  
}
#endif
/** @} */ // end of doxygen group 

