//by vic
#ifndef __PCASEGMENTATIONCOMBINE_H__
#define __PCASEGMENTATIONCOMBINE_H__


#include "ImageClusterBase.h"
#include "TTree.h"
#include "ClusterParams.h"
#include "PCABox.h"


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
      
    //Doesn't have to be so heavy
    Point2D _cntr_pt;
    std::vector<Point2D> _eigen_vecs;
    std::vector<double>  _eigen_val;
    std::vector<double>  _line;
    std::pair<int,int> _trunk_index;

    double _cov_breakup;

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
    double _angle_cut;
    
    void clear_vars();

    
    void connect(const PCABox& box,                  //incoming box to compare too
		 const std::vector<PCABox>& boxes,   //list of all boxes
		 std::map<int,bool>& used,
		 const std::map<int,std::vector<int> >& neighbors,
		 std::map<int,std::vector<int> >& combined,
		 int i,int k);
    

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

