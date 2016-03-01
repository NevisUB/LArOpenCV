//by vic
#ifndef __PCASEGMENTATION_H__
#define __PCASEGMENTATION_H__


#include "ClusterAlgoBase.h"
#include "TTree.h"
#include "ClusterParams.h"
#include "PCABox.h"
#include "ClusterAlgoFactory.h"
namespace larcv {
  /**
     \class PCASegmentation
     @brief A class with segmented PCA
  */
  class PCASegmentation : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PCASegmentation(const std::string name="PCASegmentation") :
      ClusterAlgoBase(name),
      _outtree(nullptr)
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
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);
    
    
  private:
      
    //depricated (?)
    Point2D _cntr_pt;
    std::vector<Point2D> _eigen_vecs;
    std::vector<double>  _eigen_val;
    std::vector<double>  _line;
    std::pair<int,int>  _trunk_index;
    //depricated (?)
    
    double _cov_cut;

    int _segments_x;
    int _segments_y;

    int _nhits_cut;
    int _sub_nhits_cut;
    
    double _pearsons_r;

    int _n_divisions;
    
    double _eval1;
    double _eval2;
    
    double _area;
    double _perimeter;

    double _angle_cut;

    bool _allow_cross_empties;
    int _n_clustersize;
    
    void clear_vars();

    // recursive connection function which traverses the graph
    // and runs PCAUtilities::connect function while checking
    // PCAUtilities::compatible
    void connect(const PCABox& box,                  //incoming box to compare too
		 const std::vector<PCABox>& boxes,   //list of all boxes
		 std::map<int,bool>& used,
		 const std::map<int,std::vector<int> >& neighbors,
		 std::map<int,std::vector<int> >& combined,
		 int i,int k);

    // recusively puts box into boxes, if linearity is below a cut value then it 
    // subdivides given ndivisions
    void check_linearity(PCABox& box, std::vector<PCABox>& boxes,int ndivisions);

    // you can choose in fcl file to cross neighbors or not if they are empty.
    void cross_empty_neighbors(const std::vector<PCABox>& boxes,
			       std::map<int,std::vector<int> >& neighbors);
      
  };
  
  /**
     \class larcv::PCASegmentationFactory
     \brief A concrete factory class for larcv::PCASegmentation
   */
  class PCASegmentationFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PCASegmentationFactory() { ClusterAlgoFactory::get().add_factory("PCASegmentation",this); }
    /// dtor
    ~PCASegmentationFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PCASegmentation(instance_name); }
  };
  /// Global larcv::PCASegmentationFactory to register ClusterAlgoFactory
  static PCASegmentationFactory __global_PCASegmentationFactory__;
}
#endif
/** @} */ // end of doxygen group 

