/**
 * \file ImageClusterFactory.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ImageClusterFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef IMAGECLUSTERFACTORY_H
#define IMAGECLUSTERFACTORY_H

#include <iostream>
#include <map>
#include "Core/laropencv_base.h"
#include "Core/larbys.h"
namespace larcv {

  class ImageClusterBase;

  /**
     \class ImageClusterFactoryBase
     \brief Abstract base class for factory (to be implemented per algorithm)
   */
  class ImageClusterFactoryBase {
  public:
    /// Default ctor
    ImageClusterFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~ImageClusterFactoryBase(){}
    /// Abstract constructor method
    virtual ImageClusterBase* create(const std::string instance_name) = 0;
  };

  /**
     \class ImageClusterFactory
     \brief Factory class for instantiating algorithm instance by larcv::ImageClusterManager
     This factory class can instantiate a specified algorithm instance w/ provided instance name. \n
     The actual factory core (to which each algorithm must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larcv::ImageClusterFactory::get) \n
     to access a factory.
  */
  class ImageClusterFactory : public laropencv_base  {

  public:
    /// Default ctor, shouldn't be used
    ImageClusterFactory() : laropencv_base("ImageClusterFactory")
    {}
    /// Default dtor
    ~ImageClusterFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    static ImageClusterFactory& get()
    { if(!_me) _me = new ImageClusterFactory; return *_me; }
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(const std::string name, larcv::ImageClusterFactoryBase* factory)
    { _factory_map[name] = factory; }
    /// Factory creation method (should be called by clients, possibly you!)
    ImageClusterBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LARCV_ERROR((*this)) << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }
  private:
    /// Static factory container
    std::map<std::string,larcv::ImageClusterFactoryBase*> _factory_map;
    /// Static self
    static ImageClusterFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

