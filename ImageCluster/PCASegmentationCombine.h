/**
 * \file PCASegmentationCombine.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCASegmentationCombine
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PCASEGMENTATIONCOMBINE_H__
#define __PCASEGMENTATIONCOMBINE_H__

#include "ImageClusterBase.h"
#include "TTree.h"
#include "ClusterParams.h"

#include <array>


namespace larcv {
  /**
     \class PCASegmentationCombine
     @brief A class with segmented PCA
  */
  class PCASegmentationCombine : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    PCASegmentationCombine(const std::string name="PCASegmentationCombine") :
      ImageClusterBase(name),
      _outtree(nullptr),
      _min_trunk_length(1),
      _trunk_deviation(10),
      _closeness(10)
    {}
    
    /// Default destructor
    ~PCASegmentationCombine(){}
    
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
     \class larcv::PCASegmentationCombineFactory
     \brief A concrete factory class for larcv::PCASegmentationCombine
   */
  class PCASegmentationCombineFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    PCASegmentationCombineFactory() { ImageClusterFactory::get().add_factory("PCASegmentationCombine",this); }
    /// dtor
    ~PCASegmentationCombineFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PCASegmentationCombine(instance_name); }
  };
  /// Global larcv::PCASegmentationCombineFactory to register ImageClusterFactory
  static PCASegmentationCombineFactory __global_PCASegmentationCombineFactory__;
}
#endif
/** @} */ // end of doxygen group 

