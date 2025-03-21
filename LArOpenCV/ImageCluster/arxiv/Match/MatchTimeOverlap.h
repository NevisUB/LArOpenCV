/**
 * \file MatchTimeOverlap.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MatchTimeOverlap
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __MATCHTIMEOVERLAP_H__
#define __MATCHTIMEOVERLAP_H__

#include "LArOpenCV/ImageCluster/Base/MatchAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class MatchTimeOverlap
     @brief A simple matching algorithm meant to serve for testing/example by Kazu
  */
  class MatchTimeOverlap : public larocv::MatchAlgoBase {
    
  public:
    
    /// Default constructor
    MatchTimeOverlap(const std::string name="MatchTimeOverlap") : MatchAlgoBase(name)
    {}
    
    /// Default destructor
    ~MatchTimeOverlap(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    double _Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& roi_vtx); 
		     
    
  private:
    float _time_ratio_cut ;
    float _start_time_cut ;
    bool _require_3planes;

    double _pixel_y(const Cluster2D* cluster,size_t pix);

    bool _use_only_2planes; 
    
  };
  
  /**
     \class larocv::MatchTimeOverlapFactory
     \brief A concrete factory class for larocv::MatchTimeOverlap
   */
  class MatchTimeOverlapFactory : public AlgoFactoryBase {
  public:
    /// ctor
    MatchTimeOverlapFactory() { AlgoFactory::get().add_factory("MatchTimeOverlap",this); }
    /// dtor
    ~MatchTimeOverlapFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new MatchTimeOverlap(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

