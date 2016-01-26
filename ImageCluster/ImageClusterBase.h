/**
 * \file ImageClusterBase.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ImageClusterBase
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __IMAGECLUSTERBASE_H__
#define __IMAGECLUSTERBASE_H__

#include <iostream>
#include "FhiclLite/PSet.h"
#include "Core/ImageMeta.h"
#include "Utils/Watch.h"
#include "ImageClusterTypes.h"
namespace larcv {
  /**
     \class ImageClusterBase
     An abstract base class for ImageCluster algorithms. Its role is to construct a set of "cluster" given an image. \n
     An algorithm instance is expected to be "configured" and "executed" via larcv::ImageClusterManager, though a user \n
     can perform those actions by hand (which may be useful for development work) as well if wished. \n \n

     larcv::ImageClusterManager executes a chain of algorithms that inherit from ImageClusterBase. Each algorithm instance \n
     receives an image, information about previous algorithm's clustering result, and meta data to be updated for resulting \n
     clusters from it. See larcv::ImageClusterManager for more details.\n\n

     Configuration parameters are passed via fhicllite::PSet (which is constructed via larcv::ImageClusterManager) \n
     and a derived class must implement ImageClusterBase::Configure abstruct method. An execution of an algorithm \n
     is done inside an abstract method ImageClusterBase::_Process_ method which must be implemented in a child class. \n
     A cluster is represented as a 2D contour (larcv::Contour_t). The execution method recieves 3 inputs: 0) a set of \n
     pre-defined clusters (typically a set of clusters created by the previous algorithm in a chain executed by \n
     larcv::ImageClusterManager), 1) cv::Mat representing 2D image, and 2) larcv::ImageMeta which contains meta data \n
     of 1) and should be updated to interpret the algorithm's return larcv::Contour_t if needed. \n \n
  */
  class ImageClusterBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larcv::ImageClusterManager
    ImageClusterBase(const std::string name = "noname");
    
    /// Default destructor
    virtual ~ImageClusterBase(){}

    /// Name accessor
    const std::string& Name() const { return _name; };

    /// Configuration method
    virtual void Configure(const ::fcllite::PSet &pset) = 0;

    /**
       @brief wrapper execution method: internally executes _Process_ function (see there for details)
    */
    larcv::ContourArray_t Process(const larcv::ContourArray_t& clusters,
				  const ::cv::Mat& img,
				  larcv::ImageMeta& meta);

    /// Process time getter
    double AverageProcessTime() const { return _proc_time / _proc_count; }

  protected:

    /**
       Execution method: given previous algorithm's output clusters, image, and metadata to be updated, produce clusters. \n
       Each cluster is represented by larcv::Contour_t. The first argument is clusters created by a previous \n
       algorithm's execution (ignore if not needed). The second argumnet is the image to be wokred on. The \n
       third argument is meta data to interpret a returned clusters (i.e. update metadata if return clusters \n
       do not use the same coordinate reference as provided image, else leave unchanged).
     */
    virtual larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta) = 0;

  private:

    std::string _name;   ///< name identifier, used to fetch configuration

    larcv::Watch _watch; ///< algorithm profile stopwatch

    double _proc_time;   ///< algorithm execution time record (cumulative)

    double _proc_count;  ///< algorithm execution counter (cumulative)

  };
}
#endif
/** @} */ // end of doxygen group 

