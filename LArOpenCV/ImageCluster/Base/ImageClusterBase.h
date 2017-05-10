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
#include <sstream> 

#include <TFile.h>

#include "ImageClusterFMWKInterface.h"
#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/Core/ROI.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "Watch.h"
#include "ImageClusterTypes.h"
#include "AlgoDataManager.h"

namespace larocv {

  class ImageClusterManager;
  
  /**
     \class ImageClusterBase
     An abstract base class for ImageCluster algorithms. Its role is to construct a set of "cluster" given an image. \n
     An algorithm instance is expected to be "configured" and "executed" via larocv::ImageClusterManager, though a user \n
     can perform those actions by hand (which may be useful for development work) as well if wished. \n \n

     larocv::ImageClusterManager executes a chain of algorithms that inherit from ImageClusterBase. Each algorithm instance \n
     receives an image, information about previous algorithm's clustering result, and meta data to be updated for resulting \n
     clusters from it. See larocv::ImageClusterManager for more details.\n\n

     Configuration parameters are passed via fhicllite::PSet (which is constructed via larocv::ImageClusterManager) \n
     and a derived class must implement ImageClusterBase::_Configure_ abstruct method. An execution of an algorithm \n
     is done inside an abstract method ImageClusterBase::_Process_ method which must be implemented in a child class. \n
     A cluster is represented as a 2D contour (larocv::Contour_t). The execution method recieves 3 inputs: 0) a set of \n
     pre-defined clusters (typically a set of clusters created by the previous algorithm in a chain executed by \n
     larocv::ImageClusterManager), 1) cv::Mat representing 2D image, and 2) larocv::ImageMeta which contains meta data \n
     of 1) and should be updated to interpret the algorithm's return larocv::Contour_t if needed. \n \n
  */
  class ImageClusterBase : public laropencv_base{

    friend class ImageClusterManager;
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ImageClusterBase(const std::string name = "noname");
    
    /// Default destructor
    virtual ~ImageClusterBase(){}

    /// Name accessor
    const std::string& Name() const { return _name; }

    /// Algorithm ID accessor
    AlgorithmID_t ID(const std::string& name="") const;

    /// ImageSet ID accessor
    ImageSetID_t DefaultImageSetID() const;

    /// Profile flag setter
    void Profile(bool doit) { _profile = doit; }

    /// Profile flag getter
    bool Profile() const { return _profile; }

    /// Process count
    size_t ProcessCount() const { return _proc_count; }

    /// Process time
    double ProcessTime() const { return _proc_time; }

    /// Configuration method
    virtual void Configure(const Config_t &pset);

    /// Reset method, not to re-configure but called per-event
    virtual void Reset() = 0;

    /// Finalize after (possibly multiple) process call. Handed TFile may be used to store objects.
    virtual void Finalize(TFile*) = 0;

    /// Algorithm type
    virtual AlgorithmType_t Type() const = 0;

    /// Total # of image count
    size_t NImages() const { return _num_images; }

    /// Access to ANY algorithm's data (const reference)
    template <class T>
    const T& AlgoData(AlgorithmID_t algo_id, Index_t index) const
    {
      if(!_dataman_ptr) throw larbys("AlgoDataManager not available");
      return *( (T*)(_dataman_ptr->Data(algo_id,index)));
    }

    /// Access to OWN algorithm's data (non-const reference)
    template <class T>
    T& AlgoData(Index_t index) 
    {
      if(!_dataman_ptr) throw larbys("AlgoDataManager not available");
      return *( (T*)(_dataman_ptr->Data(_id,index)));
    }

    /// Access to association manager (read-only)
    const data::AlgoDataAssManager& AssManager() const { return _dataman_ptr->AssManager(); }

    /// Association storage method to create one-to-one association
    void AssociateOne(const data::AlgoDataArrayElementBase& obj_a, const data::AlgoDataArrayElementBase& obj_b)
    { _dataman_ptr->AssociateOne(obj_a,obj_b); }

    /// Association storage method to create many-to-many association
    void AssociateMany(const data::AlgoDataArrayElementBase& obj_a, const data::AlgoDataArrayElementBase& obj_b)
    { _dataman_ptr->AssociateMany(obj_a,obj_b); }

    /// Access method for a set of images (by id)
    std::vector<cv::Mat> ImageArray(ImageSetID_t image_id=ImageSetID_t::kImageSetUnknown) const;

    /// Access method for a set of meta associated with images (by id)
    const std::vector<larocv::ImageMeta>& MetaArray(ImageSetID_t image_id=ImageSetID_t::kImageSetUnknown) const;

    /// Access method for a set of ROI associated with images (by id)
    const std::vector<larocv::ROI>& ROIArray(ImageSetID_t image_id=ImageSetID_t::kImageSetUnknown) const;

    /// Attach RSEE
    void AttachIDs(TTree* tree);
    
  protected:

    void Register(data::AlgoDataBase* data);

    larocv::Watch _watch; ///< algorithm profile stopwatch

    double _proc_time;   ///< algorithm execution time record (cumulative)

    size_t _proc_count;  ///< algorithm execution counter (cumulative)

  private:

    size_t _num_images; /// # images to be processed

    larocv::ImageSetID_t _image_set_id; ///< unique identifier for a collection of an image

    larocv::AlgorithmID_t _id; ///< unique algorithm identifier 

    std::string _name;   ///< name identifier, used to fetch configuration

    bool _profile;       ///< measure process time if profile flag is on

    data::AlgoDataManager* _dataman_ptr; ///< pointer collection to AlgoDataManager

    void SetData(const std::vector<std::vector<cv::Mat> >& image_vv,
		 const std::vector<std::vector<larocv::ImageMeta> >& meta_vv,
		 const std::vector<std::vector<larocv::ROI> >& roi_vv);

    std::vector<std::vector<cv::Mat> >           _image_vv;  ///< image data container

    std::vector<std::vector<larocv::ImageMeta> > _meta_vv; ///< meta data container
    
    std::vector<std::vector<larocv::ROI> >       _roi_vv; ///< ROI data container

  };

}
#endif
/** @} */ // end of doxygen group 

