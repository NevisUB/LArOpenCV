#ifndef __VERTEXSEEDRESIZE_H__
#define __VERTEXSEEDRESIZE_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: XYZ
*/

namespace larocv {

  class VertexSeedResize : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexSeedResize(const std::string name = "VertexSeedResize") :
      ImageAnaBase(name)
    {
      _thresh=kINVALID_FLOAT;
      _start_rad=kINVALID_FLOAT;
      _end_rad=kINVALID_FLOAT;
      _step=kINVALID_FLOAT;
    }
    
    /// Default destructor
    virtual ~VertexSeedResize(){}

    /// Finalize after process
    void Finalize(TFile*);

    void Reset() {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;
    
  private:
    AlgorithmID_t _vertexseed_id;
    
    float _thresh;
    float _start_rad;
    float _end_rad;
    float _step;

    msg::Level_t _slogger_level;
  };

  /**
     \class VertexSeedResizeFactory
     \brief A concrete factory class for VertexSeedResize
  */
  class VertexSeedResizeFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexSeedResizeFactory() { AlgoFactory::get().add_factory("VertexSeedResize",this); }
    /// dtor
    ~VertexSeedResizeFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexSeedResize(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 
