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

  class ImageClusterFactoryBase {
  public:
    ImageClusterFactoryBase(){}
    virtual ~ImageClusterFactoryBase(){}
    virtual ImageClusterBase* create(const std::string instance_name) = 0;
  };

  /**
     \class ImageClusterFactory
     User defined class ImageClusterFactory ... these comments are used to generate
     doxygen documentation!
  */
  class ImageClusterFactory : public laropencv_base  {

  public:
    ImageClusterFactory() : laropencv_base("ImageClusterFactory")
    {}

    ~ImageClusterFactory() {_factory_map.clear();}

    static ImageClusterFactory& get()
    { if(!_me) _me = new ImageClusterFactory; return *_me; }

    void add_factory(const std::string name, larcv::ImageClusterFactoryBase* factory)
    { _factory_map[name] = factory; }
    
    ImageClusterBase* create(const std::string name, const std::string instance_name) {
      auto iter = _factory_map.find(name);
      if(iter == _factory_map.end() || !((*iter).second)) {
	LARCV_ERROR((*this)) << "Found no registered class " << name << std::endl;
	return nullptr;
      }
      return (*iter).second->create(instance_name);
    }
  private:
    std::map<std::string,larcv::ImageClusterFactoryBase*> _factory_map;
    static ImageClusterFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

