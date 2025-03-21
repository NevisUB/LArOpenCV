//by vic
#ifndef __PCASEGMENTATION_H__
#define __PCASEGMENTATION_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

#include "TTree.h"
//#include "ClusterParams.h"
#include "PCABox.h"
#include "PCAPath.h"

namespace larocv {
  /**
     \class PCASegmentation
     @brief A class with segmented PCA
  */
  class PCASegmentation : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PCASegmentation(const std::string name="PCASegmentation") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PCASegmentation(){}
    
  private:

  public:
    void Finalize(TFile* fout) { }

    std::vector<PCAPath> _pcapaths; //one for each output contour
    
  protected:
    
    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
    
  private:
    
    double _cov_cut;
    
    int _segments_x;
    int _segments_y;
    
    int _nhits_cut;
    int _sub_nhits_cut;
    
    int _n_divisions;
    
    double _angle_cut;
    
    bool _allow_cross_empties;
    int _n_clustersize;
    int _n_path_size;
    
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
    Point2D point_farthest_away(Cluster2D& ocluster,
				const Point2D& startpoint);
  };
  
  /**
     \class larocv::PCASegmentationFactory
     \brief A concrete factory class for larocv::PCASegmentation
   */
  class PCASegmentationFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PCASegmentationFactory() { AlgoFactory::get().add_factory("PCASegmentation",this); }
    /// dtor
    ~PCASegmentationFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PCASegmentation(instance_name); }
  };
  /// Global larocv::PCASegmentationFactory to register AlgoFactory
  static PCASegmentationFactory __global_PCASegmentationFactory__;
}
#endif
/** @} */ // end of doxygen group 

