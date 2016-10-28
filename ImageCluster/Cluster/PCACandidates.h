//by vic

#ifndef __PCACANDIDATES_H__
#define __PCACANDIDATES_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

#include "Core/Geo2D.h"

namespace larocv {
 
  class PCACandidates : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACandidates(const std::string name = "PCACandidates") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~PCACandidates(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    uint _defect_cluster_algo_id;
    
  };

  /**
     \class PCACandidatesData
     @brief only for vic
  */
  class PCACandidatesData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    PCACandidatesData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {  Clear(); }
    
    /// Default destructor
    ~PCACandidatesData(){}

    /// Clear method override

    AlgorithmID_t _input_id;
    std::vector<std::vector<std::vector<geo2d::Line<float> > > >   _ctor_lines_v_v_v;
    std::vector<std::vector<geo2d::Line<float> > >  _ctor_lines_v_v;
    std::vector<std::vector<geo2d::Vector<float> > > _ipoints_v_v;
    std::vector<std::vector<std::vector<size_t > > > _atomic_id_v_v_v;


    void Clear() {
      _ctor_lines_v_v_v.clear();
      _ctor_lines_v_v.clear();
      _ipoints_v_v.clear();
      _atomic_id_v_v_v.clear();

      _ctor_lines_v_v_v.resize(3);
      _ctor_lines_v_v.resize(3);
      _ipoints_v_v.resize(3);
      _atomic_id_v_v_v.resize(3);

      _input_id = kINVALID_ID;
    }

  };

  
  /**
     \class larocv::PCACandidatesFactory
     \brief A concrete factory class for larocv::PCACandidates
  */
  class PCACandidatesFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PCACandidatesFactory() { AlgoFactory::get().add_factory("PCACandidates",this); }
    /// dtor
    ~PCACandidatesFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new PCACandidates(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new PCACandidatesData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

