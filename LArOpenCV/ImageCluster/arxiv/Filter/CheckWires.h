/**
 * \file CheckWires.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CheckWires
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHECKWIRES_H__
#define __CHECKWIRES_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class CheckWires
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CheckWires : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    CheckWires(const std::string name="CheckWires") : ClusterAlgoBase(name)
    { _wires_v.resize(3); }
    
    /// Default destructor
    ~CheckWires(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);
    
  private:

  std::vector<std::vector<std::pair<int,int> > > _wires_v;
  std::vector<std::pair<int,int> > LoadWires(ImageMeta& meta);

  };
  
  /**
     \class larocv::CheckWiresFactory
     \brief A concrete factory class for larocv::CheckWires
   */
  class CheckWiresFactory : public AlgoFactoryBase {
  public:
    /// ctor
    CheckWiresFactory() { AlgoFactory::get().add_factory("CheckWires",this); }
    /// dtor
    ~CheckWiresFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new CheckWires(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

