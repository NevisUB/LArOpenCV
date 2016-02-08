/**
 * \file PCASegmentation.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCASegmentation
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PCASEGMENTATION_H__
#define __PCASEGMENTATION_H__

#include "ImageClusterBase.h"
#include "TTree.h"
#include "ClusterParams.h"

#include <array>


namespace larcv {
  /**
     \class PCASegmentation
     @brief A class with segmented PCA
  */
  class PCASegmentation : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    PCASegmentation(const std::string name="PCASegmentation") :
      ImageClusterBase(name),
      _outtree(nullptr),
      _min_trunk_length(1),
      _trunk_deviation(10),
      _closeness(10)
    {}
    
    /// Default destructor
    ~PCASegmentation(){}
    
  private:
    TTree* _outtree;

  public:
    void Finalize(TFile* fout) { _outtree->Write(); }

    std::vector<ClusterParams> _cparms_v;
  protected:
    
    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    



    
  private:
    std::pair<double,double> closest_point_on_line(std::array<double,4>& line,int lx,int ly);    
    double distance_to_line(std::array<double,4>& line,int lx,int ly);

      
    //Doesn't have to be so heavy
    Point2D _cntr_pt;
    std::vector<Point2D> _eigen_vecs;
    std::vector<double>  _eigen_val;
    std::array<double,4> _line;
    std::pair<int,int> _trunk_index;


    int _segments_x;
    int _segments_y;

    int _nhits_cut;
    
    double _pearsons_r;
    
    int    _min_trunk_length;
    double _trunk_deviation;

    double _trunk_length;
    double _trunk_cov;
    
    double _eval1;
    double _eval2;
    
    double _area;
    double _perimeter;

    double _closeness;
    
    double stdev( std::vector<int>& data,
		  size_t start, size_t end );
    
    double cov ( std::vector<int>& data1,
		 std::vector<int>& data2,
		 size_t start, size_t end ) ; 
    
    double mean( std::vector<int>& data,
		 size_t start, size_t end );

    void clear_vars();

    bool pca_line(const ::cv::Mat& subimg,
		  Contour_t cluster_s,
		  const ::cv::Rect& rect,
		  std::vector<double>& line);

    int get_charge_sum(const ::cv::Mat& subImg,
		       const Contour_t& pts);

    std::pair<double,double> get_mean_loc(const ::cv::Rect& rect,
					  const Contour_t& pts );

    double get_roi_cov(const Contour_t & pts);
  };
  
  /**
     \class larcv::PCASegmentationFactory
     \brief A concrete factory class for larcv::PCASegmentation
   */
  class PCASegmentationFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    PCASegmentationFactory() { ImageClusterFactory::get().add_factory("PCASegmentation",this); }
    /// dtor
    ~PCASegmentationFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PCASegmentation(instance_name); }
  };
  /// Global larcv::PCASegmentationFactory to register ImageClusterFactory
  static PCASegmentationFactory __global_PCASegmentationFactory__;
}
#endif
/** @} */ // end of doxygen group 

