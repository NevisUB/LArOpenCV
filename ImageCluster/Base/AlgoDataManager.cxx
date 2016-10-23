#ifndef ALGODATAMANAGER_CXX
#define ALGODATAMANAGER_CXX

#include "Core/larbys.h"
#include "AlgoDataManager.h"
#include <sstream>

namespace larocv {

  AlgoDataManager::AlgoDataManager()
    : laropencv_base("AlgoDataManager")
    , _tree_attached(false)
    , _tree(nullptr)
    , _data_v()
  {}

  void AlgoDataManager::Register(AlgoDataBase* data)
  {
    LAROCV_NORMAL() << "Registering AlgoData " << data->Name() << " ID = " << data->ID() << std::endl;
    if(_tree_attached)
      throw larbys("Cannot register after TTree attachment happened!");    
    if(data->ID() >= _data_v.size()) {
      _data_v.resize(data->ID()+1,nullptr);
      _name_v.resize(data->ID()+1,"");
    }
    if(_data_v[data->ID()]){
      std::stringstream ss;
      ss << "Duplicate registration of AlgoDataBase pointer (ID=" << data->ID() <<")!";
      throw larbys(ss.str());
    }
    _data_v[data->ID()] = data;
    _name_v[data->ID()] = data->Name();
  }

  AlgorithmID_t AlgoDataManager::ID(const std::string& name)
  {
    AlgorithmID_t id=kINVALID_ALGO_ID;
    for(size_t i=0; i<_name_v.size(); ++i) {
      if(_name_v[i] != name) continue;
      if(id == kINVALID_ALGO_ID) id = i;
      else {
	std::stringstream ss;
	ss << "Algorithm name \"" << name << "\" is duplicated!" << std::endl;
	throw larbys(ss.str());
      }
    }
    return id;
  }

  void AlgoDataManager::Register(TTree* tree)
  {
    _tree = tree;
    for(size_t i=0; i<_data_v.size(); ++i) {
      auto& ptr = _data_v[i];
      if(!ptr) continue;
      std::stringstream ss;
      ss << "algo" << ptr->ID() << "_" << ptr->Name().c_str() << "_branch";
      std::string name(ss.str());
      _tree->Branch(name.c_str(),&(_data_v[i]));
    }
    _tree_attached = true;
  }

  const AlgoDataBase* AlgoDataManager::Data(AlgorithmID_t id)
  {
    if( id >= _data_v.size() ) {
      std::stringstream ss;
      ss << "Invalid AlgorithmID_t (" << id << ") requested for AlgoData access!";
      throw larbys(ss.str());
    }
    if(!_data_v[id]) {
      std::stringstream ss;
      ss << "AlgorithmID_t (" << id << ") does not have any registered data!";
      throw larbys(ss.str());
    }
    return _data_v[id];
  }
}
#endif
