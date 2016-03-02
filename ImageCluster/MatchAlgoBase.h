/**
 * \file MatchAlgoBase.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MatchAlgoBase
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __MATCHALGOBASE_H__
#define __MATCHALGOBASE_H__

#include "ImageClusterBase.h"
#include "Cluster2D.h"

namespace larcv {

  /**
     \class MatchAlgoBase
     An abstract base class for ImageCluster algorithms. Its role is to construct a set of "cluster" given an image. \n
     An algorithm instance is expected to be "configured" and "executed" via larcv::ImageClusterManager, though a user \n
     can perform those actions by hand (which may be useful for development work) as well if wished. \n \n

     larcv::ImageClusterManager executes a chain of algorithms that inherit from MatchAlgoBase. Each algorithm instance \n
     receives an image, information about previous algorithm's clustering result, and meta data to be updated for resulting \n
     clusters from it. See larcv::ImageClusterManager for more details.\n\n

     Configuration parameters are passed via fhicllite::PSet (which is constructed via larcv::ImageClusterManager) \n
     and a derived class must implement MatchAlgoBase::_Configure_ abstruct method. An execution of an algorithm \n
     is done inside an abstract method MatchAlgoBase::_Process_ method which must be implemented in a child class. \n
     A cluster is represented as a 2D contour (larcv::Cluster2D). The execution method recieves 3 inputs: 0) a set of \n
     pre-defined clusters (typically a set of clusters created by the previous algorithm in a chain executed by \n
     larcv::ImageClusterManager), 1) cv::Mat representing 2D image, and 2) larcv::ImageMeta which contains meta data \n
     of 1) and should be updated to interpret the algorithm's return larcv::Cluster2D if needed. \n \n
  */
  class MatchAlgoBase : public ImageClusterBase {

  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larcv::ImageClusterManager
    MatchAlgoBase(const std::string name = "noname");
    
    /// Default destructor
    virtual ~MatchAlgoBase(){}

    /// Configuration method
    void Configure(const ::fcllite::PSet &pset);

    /**
       @brief wrapper execution method: internally executes _Process_ function (see there for details)
    */
    double Process(const larcv::Cluster2DPtrArray_t& clusters);

  protected:

    /// Inherited class configuration method
    virtual void _Configure_(const ::fcllite::PSet &pset) = 0;

    /**
       Execution method: given previous algorithm's output clusters, image, and metadata to be updated, produce clusters. \n
       Each cluster is represented by larcv::Cluster2D. The first argument is clusters created by a previous \n
       algorithm's execution (ignore if not needed). The second argumnet is the image to be wokred on. The \n
       third argument is meta data to interpret a returned clusters (i.e. update metadata if return clusters \n
       do not use the same coordinate reference as provided image, else leave unchanged).
     */
    virtual double _Process_(const larcv::Cluster2DPtrArray_t& clusters)=0;
    
  };

}
#endif
/** @} */ // end of doxygen group 

