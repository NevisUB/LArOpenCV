/**
 * \file PCAProjection.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCAProjection
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PCAPROJECTION_H__
#define __PCAPROJECTION_H__

#include "ImageClusterBase.h"
#include "TTree.h"
#include <array>
#include "PCAProjectionVars.h"

namespace larcv {
  /**
     \class PCAProjection
     @brief A brief
  */
  class PCAProjection : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    PCAProjection(const std::string name="PCAProjection") :
      ImageClusterBase(name),
      _outtree(nullptr),
      _min_trunk_length(1),
      _trunk_deviation(10)
    {}
    
    /// Default destructor
    ~PCAProjection(){}
    
  private:
    TTree* _outtree;

  public:
    void Finalize(TFile* fout) { _outtree->Write(); }
    
  protected:
    
    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:



    //Doesn't have to be so heavy
    Point2D _cntr_pt;
    std::vector<Point2D> _eigen_vecs;
    std::vector<double>  _eigen_val;
    std::array<double,4> _line;
    std::pair<int,int> _trunk_index;

    double _pearsons_r;
    
    int    _min_trunk_length;
    double _trunk_deviation;

    double _trunk_length;
    double _trunk_cov;
    
    double _eval1;
    double _eval2;
    
    double _area;
    double _perimeter;

    double stdev( std::vector<int>& data,
		  size_t start, size_t end );
    
    double cov ( std::vector<int>& data1,
		 std::vector<int>& data2,
		 size_t start, size_t end ) ; 
    
    double mean( std::vector<int>& data,
		 size_t start, size_t end );

    void clear_vars();
  };
  
  /**
     \class larcv::PCAProjectionFactory
     \brief A concrete factory class for larcv::PCAProjection
   */
  class PCAProjectionFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    PCAProjectionFactory() { ImageClusterFactory::get().add_factory("PCAProjection",this); }
    /// dtor
    ~PCAProjectionFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PCAProjection(instance_name); }
    /// variable container method
    AlgoVarsBase* vars() { return new PCAProjectionVars; }
  };
  /// Global larcv::PCAProjectionFactory to register ImageClusterFactory
  static PCAProjectionFactory __global_PCAProjectionFactory__;
}
#endif
/** @} */ // end of doxygen group 

