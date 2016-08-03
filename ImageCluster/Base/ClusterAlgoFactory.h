/**
 * \file ClusterAlgoFactory.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ClusterAlgoFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef CLUSTERALGOFACTORY_H
#define CLUSTERALGOFACTORY_H

#include <iostream>
#include <map>
#include "Core/laropencv_base.h"
#include "Core/larbys.h"

namespace larocv {

  class ClusterAlgoBase;
  /**
     \class ClusterAlgoFactoryBase
     \brief Abstract base class for factory (to be implemented per algorithm)
  */
  class ClusterAlgoFactoryBase {
  public:
    /// Default ctor
    ClusterAlgoFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~ClusterAlgoFactoryBase(){}
    /// Abstract constructor method
    virtual ClusterAlgoBase* create(const std::string instance_name) = 0;
  };

  /**
     \class ClusterAlgoFactory
     \brief Factory class for instantiating algorithm instance by larocv::ImageClusterManager
     This factory class can instantiate a specified algorithm instance w/ provided instance name. \n
     The actual factory core (to which each algorithm must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larocv::ClusterAlgoFactory::get) \n
     to access a factory.
  */
  class ClusterAlgoFactory : public laropencv_base  {

  public:
    /// Default ctor, shouldn't be used
    ClusterAlgoFactory() : laropencv_base("ClusterAlgoFactory")
    {}
    /// Default dtor
    ~ClusterAlgoFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    static ClusterAlgoFactory& get()
    { if(!_me) _me = new ClusterAlgoFactory; return *_me; }
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(const std::string name, larocv::ClusterAlgoFactoryBase* factory)
    { _factory_map[name] = factory; }
    /// Factory creation method (should be called by clients, possibly you!)
    ClusterAlgoBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LAROCV_ERROR((*this)) << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }

  private:
    /// Static factory container
    std::map<std::string,larocv::ClusterAlgoFactoryBase*> _factory_map;
    /// Static self
    static ClusterAlgoFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

