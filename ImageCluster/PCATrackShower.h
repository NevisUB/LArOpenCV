/**
 * \file PCATrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCATrackShower
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PCATRACKSHOWER_H__
#define __PCATRACKSHOWER_H__

#include "ImageClusterBase.h"
#include "TTree.h"
#include <array>


struct _object;
typedef _object PyObject;

#ifndef __CINT__
#include "Python.h"
#include "numpy/arrayobject.h"
#endif

#include "Utils/NDArrayConverter.h"


namespace larcv {
  /**
     \class PCATrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PCATrackShower : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    PCATrackShower(const std::string name="PCATrackShower") :
      ImageClusterBase(name),
      _outtree(nullptr),
      _min_trunk_length(1),
      _trunk_deviation(10)
    {}
    
    /// Default destructor
    ~PCATrackShower(){}

  private:
    TTree* _outtree;

  public:
    void Finalize(TFile* fout) { _outtree->Write(); }
    
    std::vector<Point2D> _cntr_pt_v;
    std::vector<std::vector<Point2D> > _eigen_vecs_v;
    std::vector<std::vector<double> >  _eigen_val_v;

    int SumImageSize() { return _subMat_v.size(); }
      
    PyObject* GetSubImage(int i)
    { ::larcv::convert::NDArrayConverter converter; return converter.toNDArray( _subMat_v.at(i) ); }

    std::vector<std::array<double,4> > _line_v;

    std::vector<std::vector<std::array<double,4> > > _dlines_v;
    std::vector<std::vector<double > > _ddists_v;

    std::vector< std::vector< std::pair<int,double> > > _ddd_v;

    std::vector<std::pair<int,int> > _trunk_index_v;
    std::vector<double> _pearsons_r_v;
    std::vector<double> _trunk_len_v;
    
  protected:
    
    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    int    _min_trunk_length;
    double _trunk_deviation;

    double _trunk_length;
    double _trunk_cov;
    
    double _eval1;
    double _eval2;
    
    double _area;
    double _perimeter;

    std::vector<::cv::Mat> _subMat_v;
    std::vector<double> _dists_v;

    double stdev( std::vector<int>& data,
		  size_t start, size_t end );
    
    double cov ( std::vector<int>& data1,
		 std::vector<int>& data2,
		 size_t start, size_t end ) ; 
   
    double mean( std::vector<int>& data,
		 size_t start, size_t end );
      
  };
  
  /**
     \class larcv::PCATrackShowerFactory
     \brief A concrete factory class for larcv::PCATrackShower
   */
  class PCATrackShowerFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    PCATrackShowerFactory() { ImageClusterFactory::get().add_factory("PCATrackShower",this); }
    /// dtor
    ~PCATrackShowerFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PCATrackShower(instance_name); }
  };
  /// Global larcv::PCATrackShowerFactory to register ImageClusterFactory
  static PCATrackShowerFactory __global_PCATrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

