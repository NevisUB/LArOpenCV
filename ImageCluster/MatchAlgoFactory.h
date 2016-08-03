/**
 * \file MatchAlgoFactory.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MatchAlgoFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef MATCHALGOFACTORY_H
#define MATCHALGOFACTORY_H

#include <iostream>
#include <map>
#include "Core/laropencv_base.h"
#include "Core/larbys.h"

namespace larocv {

  class MatchAlgoBase;
  /**
     \class MatchAlgoFactoryBase
     \brief Abstract base class for factory (to be implemented per algorithm)
  */
  class MatchAlgoFactoryBase {
  public:
    /// Default ctor
    MatchAlgoFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~MatchAlgoFactoryBase(){}
    /// Abstract constructor method
    virtual MatchAlgoBase* create(const std::string instance_name) = 0;
  };

  /**
     \class MatchAlgoFactory
     \brief Factory class for instantiating algorithm instance by larocv::ImageClusterManager
     This factory class can instantiate a specified algorithm instance w/ provided instance name. \n
     The actual factory core (to which each algorithm must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larocv::MatchAlgoFactory::get) \n
     to access a factory.
  */
  class MatchAlgoFactory : public laropencv_base  {

  public:
    /// Default ctor, shouldn't be used
    MatchAlgoFactory() : laropencv_base("MatchAlgoFactory")
    {}
    /// Default dtor
    ~MatchAlgoFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    static MatchAlgoFactory& get()
    { if(!_me) _me = new MatchAlgoFactory; return *_me; }
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(const std::string name, larocv::MatchAlgoFactoryBase* factory)
    { _factory_map[name] = factory; }
    /// Factory creation method (should be called by clients, possibly you!)
    MatchAlgoBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LAROCV_ERROR((*this)) << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }

  private:
    /// Static factory container
    std::map<std::string,larocv::MatchAlgoFactoryBase*> _factory_map;
    /// Static self
    static MatchAlgoFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

