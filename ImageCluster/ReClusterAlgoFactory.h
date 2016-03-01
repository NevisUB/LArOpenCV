/**
 * \file ReClusterAlgoFactory.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ReClusterAlgoFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef RECLUSTERALGOFACTORY_H
#define RECLUSTERALGOFACTORY_H

#include <iostream>
#include <map>
#include "Core/laropencv_base.h"
#include "Core/larbys.h"

namespace larcv {

  class ReClusterAlgoBase;
  /**
     \class ReClusterAlgoFactoryBase
     \brief Abstract base class for factory (to be implemented per algorithm)
  */
  class ReClusterAlgoFactoryBase {
  public:
    /// Default ctor
    ReClusterAlgoFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~ReClusterAlgoFactoryBase(){}
    /// Abstract constructor method
    virtual ReClusterAlgoBase* create(const std::string instance_name) = 0;
  };

  /**
     \class ReClusterAlgoFactory
     \brief Factory class for instantiating algorithm instance by larcv::ImageClusterManager
     This factory class can instantiate a specified algorithm instance w/ provided instance name. \n
     The actual factory core (to which each algorithm must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larcv::ReClusterAlgoFactory::get) \n
     to access a factory.
  */
  class ReClusterAlgoFactory : public laropencv_base  {

  public:
    /// Default ctor, shouldn't be used
    ReClusterAlgoFactory() : laropencv_base("ReClusterAlgoFactory")
    {}
    /// Default dtor
    ~ReClusterAlgoFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    static ReClusterAlgoFactory& get()
    { if(!_me) _me = new ReClusterAlgoFactory; return *_me; }
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(const std::string name, larcv::ReClusterAlgoFactoryBase* factory)
    { _factory_map[name] = factory; }
    /// Factory creation method (should be called by clients, possibly you!)
    ReClusterAlgoBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LARCV_ERROR((*this)) << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }

  private:
    /// Static factory container
    std::map<std::string,larcv::ReClusterAlgoFactoryBase*> _factory_map;
    /// Static self
    static ReClusterAlgoFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

