//by vic
#ifndef __PCASEGMENTATION_H__
#define __PCASEGMENTATION_H__


#include "ImageClusterBase.h"
#include "TTree.h"
#include "ClusterParams.h"
#include "PCABox.h"

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
      
    //Doesn't have to be so heavy
    Point2D _cntr_pt;
    std::vector<Point2D> _eigen_vecs;
    std::vector<double>  _eigen_val;
    std::vector<double>  _line;
    std::pair<int,int> _trunk_index;

    double _cov_cut;

    int _segments_x;
    int _segments_y;

    int _nhits_cut;
    int _sub_nhits_cut;
    
    double _pearsons_r;

    int _n_divisions;
    
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
    
    void check_linearity(PCABox& box, std::vector<PCABox>& boxes,int ndivisions);
    
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

