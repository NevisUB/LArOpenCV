/**
 * \file AlgoFactory.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class AlgoFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef ALGOFACTORY_H
#define ALGOFACTORY_H

#include <iostream>
#include <map>
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/Core/larbys.h"
#include "AlgoDataEmpty.h"

namespace larocv {

  class ImageClusterBase;
  /**
     \class AlgoFactoryBase
     \brief Abstract base class for factory (to be implemented per algorithm)
  */
  class AlgoFactoryBase {
  public:
    /// Default ctor
    AlgoFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~AlgoFactoryBase(){}
    /// Abstract constructor method
    virtual ImageClusterBase* create(const std::string instance_name) = 0;
  };

  /**
     \class AlgoFactory
     \brief Factory class for instantiating algorithm instance by larocv::ImageClusterManager
     This factory class can instantiate a specified algorithm instance w/ provided instance name. \n
     The actual factory core (to which each algorithm must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larocv::AlgoFactory::get) \n
     to access a factory.
  */
  class AlgoFactory : public laropencv_base  {

  public:
    /// Default ctor, shouldn't be used
    AlgoFactory() : laropencv_base("AlgoFactory")
    {}
    /// Default dtor
    ~AlgoFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    static AlgoFactory& get()
    { if(!_me) _me = new AlgoFactory; return *_me; }
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(const std::string name, larocv::AlgoFactoryBase* factory)
    { _factory_map[name] = factory; }
    /// Factory creation method (should be called by clients, possibly you!)
    ImageClusterBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LAROCV_ERROR() << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }
    
  private:
    /// Static factory container
    std::map<std::string,larocv::AlgoFactoryBase*> _factory_map;
    /// Static self
    static AlgoFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

