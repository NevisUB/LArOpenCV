#ifndef __VERTEXMERGER_H__
#define __VERTEXMERGER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include <array>

/*
  @brief: An VertexMerger
*/
namespace larocv {
 
  class VertexMerger : public larocv::ImageAnaBase {
    
  public:
    
    VertexMerger(const std::string name = "VertexMerger") :
      ImageAnaBase(name)
    {}
    
    virtual ~VertexMerger(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

    AlgorithmID_t _track_vertex_algo_id;
    AlgorithmID_t _shower_vertex_algo_id;
    AlgorithmID_t _adc_super_algo_id;


    struct VertexMeta {

      VertexMeta() {
	for(auto &v : valid_plane_v)    v = false;
	for(auto &v : weight_plane_v)   v = kINVALID_FLOAT;
	for(auto &v : ass_vtx2d_adc_v)  v = kINVALID_SIZE;
	for(auto &v : ass_cvtx2d_adc_v) v = kINVALID_SIZE;
	avg_weight = 0.0;
	nweight_planes = 0.0;
      }

      std::array<bool,3> valid_plane_v;
      std::array<float,3> weight_plane_v;
      std::array<size_t,3> ass_cvtx2d_adc_v;
      std::array<size_t,3> ass_vtx2d_adc_v;

      float avg_weight;
      size_t nweight_planes;

      std::string dump() {
	std::stringstream ss;

	ss << "\tvalid: {" ;
	for(size_t plane=0;plane<3;++plane) ss << valid_plane_v[plane] << ",";
	ss << "}" << std::endl;
	ss << "\tweight: {" ;
	for(size_t plane=0;plane<3;++plane) ss << weight_plane_v[plane] << ",";
	ss << "}" << std::endl;
	ss << "\tass_vtx2d: {" ;
	for(size_t plane=0;plane<3;++plane) ss << ass_vtx2d_adc_v[plane] << ",";
	ss << "}" << std::endl;
	ss << "\tass_cvtx2d: {" ;
	for(size_t plane=0;plane<3;++plane) ss << ass_cvtx2d_adc_v[plane] << ",";
	ss << "}" << std::endl;
	ss << "\tavg weight: " << avg_weight << " @ n planes " << nweight_planes << std::endl;
	return ss.str();
      }
    };
    
  };

  class VertexMergerFactory : public AlgoFactoryBase {
  public:
    VertexMergerFactory() { AlgoFactory::get().add_factory("VertexMerger",this); }
    ~VertexMergerFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new VertexMerger
	(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

