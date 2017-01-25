#ifndef ALGODATAMANAGER_CXX
#define ALGODATAMANAGER_CXX

#include "LArOpenCV/Core/larbys.h"
#include "AlgoDataManager.h"
#include <sstream>

namespace larocv {

  namespace data {
    
    AlgoDataManager::AlgoDataManager()
      : laropencv_base("AlgoDataManager")
      , _tree_attached(false)
      , _tree(nullptr)
      , _data_vv()
    {}

    void AlgoDataManager::Register(const AlgorithmID_t algo_id, const std::string name)
    {
      LAROCV_INFO() << "Request to register an algorithm " << name << std::endl;
      for(auto const& n : _name_v) {
	if(n != name) continue;
	std::stringstream ss;
	ss << "Duplicate algorithm name: " << name;
	throw larbys(ss.str());
      }
      if(algo_id >= _name_v.size()) {
	_name_v.resize(algo_id+1,"");
	_data_vv.resize(algo_id+1);
      }
      _name_v[algo_id] = name;
      LAROCV_NORMAL() << "Registered algorithm data collection for "
		      << name << " ID = " << _name_v.size()-1 << std::endl;
    }
    
    void AlgoDataManager::Register(const AlgorithmID_t algo_id, AlgoDataBase* data)
    {
      LAROCV_NORMAL() << "Registering AlgoData instance for ID " << algo_id << std::endl;
      if(algo_id >= _data_vv.size())
	throw larbys("Invalid algorithm id provided for AlgoData registration!");
      
      if(_tree_attached)
	throw larbys("Cannot register after TTree attachment happened!");

      _data_vv[algo_id].push_back(data);
    }
    
    AlgorithmID_t AlgoDataManager::ID(const std::string& name) const
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

    const std::string& AlgoDataManager::Name (const AlgorithmID_t algo_id) const
    {
      if( algo_id >= _data_vv.size() ) {
	std::stringstream ss;
	ss << "Invalid AlgorithmID_t (" << algo_id << ") requested for AlgoData access!";
	throw larbys(ss.str());
      }
      return _name_v[algo_id];
    }

    void AlgoDataManager::ClearData()
    {
      for(auto& data_v : _data_vv) {
	for(auto& data : data_v) {
	  data->Clear();
	}
      }
    }
    
    void AlgoDataManager::Register(TTree* tree)
    {
      _tree = tree;
      for(size_t algo_id=0; algo_id<_data_vv.size(); ++algo_id) {
	for(size_t data_id=0; data_id<_data_vv[algo_id].size(); ++data_id) {
	  std::stringstream ss;
	  ss << "algo" << algo_id
	     << "_" << _name_v[algo_id].c_str()
	     << "_data" << data_id << "_branch" << std::endl;
	  
	  std::string name(ss.str());
	  _tree->Branch(name.c_str(),&(_data_vv[algo_id][data_id]));
	}
      }
      _tree_attached = true;
    }
    
    const AlgoDataBase* AlgoDataManager::Data(const AlgorithmID_t algo_id, const AlgorithmDataID_t data_id) const
    {
      if( algo_id >= _data_vv.size() ) {
	std::stringstream ss;
	ss << "Invalid AlgorithmID_t (" << algo_id << ") requested for AlgoData access!";
	throw larbys(ss.str());
      }
      if( data_id >= _data_vv[algo_id].size() ) {
	std::stringstream ss;
	ss << "Invalid AlgorithmDataID_t (" << data_id
	   << ") requested for AlgoData access for algorithm " << Name(algo_id)
	   << " (Algorithm ID " << algo_id << ")";
	throw larbys(ss.str());
      }
      return _data_vv[algo_id][data_id];
    }
  }
}
#endif
