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

namespace larocv {

  /**
     \class ImageAnaBase
     An abstract base class for ImageAna LArOpenCV modules. \n
     Each module instance is expected to be "configured" and "executed" via larocv::ImageClusterManager\n
     larocv::ImageClusterManager executes a chain of algorithms that inherit from ImageAnaBase. Each algorithm instance \n
     can access a set of images via dedicated function as well as AlgoData data structure which is filled by any algorithm.\n
     \n

     Configuration parameters are passed via larocv::PSet (which is constructed via larocv::ImageClusterManager) \n
     and a derived class must implement ImageAnaBase::_Configure_ abstruct method. An execution of an algorithm \n
     is done inside an abstract method ImageAnaBase::_Process_ method which must be implemented in a child class. \n
     Finally there is another function, ImageAnaBase::_PostProcess_ method that is called after all modules' _Process_ \n
     function is executed. This is where, if necessary, final analysis should take place accessing any AlgoData.\n
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
    void Process();

    /**
       @brief wrapper execution method: internally executes _PostProcess_ function (see there for details)
    */
    bool PostProcess();

  protected:

    /// Configuration method (pure virtual, to be implemented)
    virtual void _Configure_(const Config_t &pset) = 0;

    /**
       Execution method: called per "event" to analyze. This is where the main data analysis/reconstruction should happen.\n
       As the function is non-const, it is allowed to access a modifi-able AlgoData owned by itself.\n
    */
    virtual void _Process_() = 0;

    /**
       Called after all LArCV modules' Process function is executed. Any clean-up or final analysis should happen here.\n
       Note it has a const-access to AlgoData (it cannot modify any longer). However it can access all AlgoData created\n
       by other modules' Process function.
     */
    virtual bool _PostProcess_() const = 0;

  private:

    std::vector<std::vector<cv::Mat> >           _image_vv;
    std::vector<std::vector<larocv::ImageMeta> > _meta_vv;
    std::vector<std::vector<larocv::ROI> >       _roi_vv;
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

