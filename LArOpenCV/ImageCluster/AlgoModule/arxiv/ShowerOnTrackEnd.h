#ifndef __SHOWERONTRACKEND_H__
#define __SHOWERONTRACKEND_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"

/*
  @brief: estimate the shower vertex point using shower seeds
*/
namespace larocv {
 
  class ShowerOnTrackEnd : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerOnTrackEnd(const std::string name = "ShowerOnTrackEnd") :
      ImageAnaBase(name),
      _VertexAnalysis()
    {}
    
    /// Default destructor
    ~ShowerOnTrackEnd(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();

    bool _PostProcess_() const;

  private:
    size_t _vertex3d_id;
    size_t _compound_id;
    VertexAnalysis _VertexAnalysis;
    double _overlap_fraction;
    double _circle_default_radius;
  };

  /**
     \class larocv::ShowerOnTrackEndFactory
     \brief A concrete factory class for larocv::ShowerOnTrackEnd
   */
  class ShowerOnTrackEndFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerOnTrackEndFactory() { AlgoFactory::get().add_factory("ShowerOnTrackEnd",this); }
    /// dtor
    ~ShowerOnTrackEndFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerOnTrackEnd(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 
