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
      , _data_id_map()
      , _data_v()
    {}

    const std::vector<std::string>& AlgoDataManager::Names() const
    { return _name_v; }

    void AlgoDataManager::Register(const AlgorithmID_t algo_id, const std::string name)
    {
      LAROCV_INFO() << "Request to register an algorithm " << name << " with ID = " << algo_id<< std::endl;
      for(auto const& n : _name_v) {
	if(n != name) continue;
	std::stringstream ss;
	ss << "Duplicate algorithm name: " << name;
	throw larbys(ss.str());
      }
      if(algo_id >= _name_v.size()) {
	_name_v.resize(algo_id+1,"");
	_data_id_map.resize(algo_id+1);
      }
      _name_v[algo_id] = name;
      LAROCV_NORMAL() << "Registered algorithm data collection for "
		      << name << " ID = " << _name_v.size()-1 << std::endl;
    }
    
    void AlgoDataManager::Register(const AlgorithmID_t algo_id, AlgoDataBase* data)
    {
      LAROCV_NORMAL() << "Registering AlgoData instance for ID " << algo_id << std::endl;
      if(algo_id >= _data_id_map.size())
	throw larbys("Invalid algorithm id provided for AlgoData registration!");
      
      if(_tree_attached)
	throw larbys("Cannot register after TTree attachment happened!");

      _data_id_map[algo_id].push_back(_data_v.size());
      data->_id = _data_v.size();
      _data_v.push_back(data);
      
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
      if( algo_id >= _data_id_map.size() ) {
	std::stringstream ss;
	ss << "Invalid AlgorithmID_t (" << algo_id << ") requested for AlgoData access!";
	throw larbys(ss.str());
      }
      return _name_v[algo_id];
    }

    void AlgoDataManager::ClearData()
    {
      for(auto& data : _data_v )
	data->Clear();
      _ass_man.ClearData();
    }

    void AlgoDataManager::Register(TTree* tree, std::vector<AlgorithmID_t> store_algo_id_v) {
      if (store_algo_id_v.empty()) {
	LAROCV_DEBUG() << "Empty algo id vector, register & store all modules" << std::endl;
	Register(tree);
	return;
      }

      _tree = tree;
      
      for(auto algo_id : store_algo_id_v) {
	for(size_t data_id=0; data_id<_data_id_map[algo_id].size(); ++data_id) {
	  std::stringstream ss;
	  ss << "algo_" << algo_id
	     << "_" << _name_v.at(algo_id).c_str()
	     << "_data_" << data_id << "_branch";
	  std::string name(ss.str());
	  _tree->Branch(name.c_str(),&(_data_v.at(_data_id_map.at(algo_id).at(data_id))));
	}
      }
      _tree->Branch("AssMan",&_ass_man);
      _tree_attached = true;
    }
    
    void AlgoDataManager::Register(TTree* tree)
    {
      _tree = tree;

      for(size_t algo_id=0; algo_id<_data_id_map.size(); ++algo_id) {
	for(size_t data_id=0; data_id<_data_id_map[algo_id].size(); ++data_id) {
	  std::stringstream ss;
	  ss << "algo_" << algo_id
	     << "_" << _name_v[algo_id].c_str()
	     << "_data" << data_id << "_branch";
	  
	  std::string name(ss.str());
	  _tree->Branch(name.c_str(),&(_data_v[_data_id_map[algo_id][data_id]]));
	}
	_tree->Branch("AssMan",&_ass_man);
	_tree_attached = true;
      }
    }
    
    AlgoDataID_t AlgoDataManager::AlgoDataID(const AlgorithmID_t algo_id, const Index_t index) const
    {
      if( algo_id >= _data_id_map.size() ) {
	std::stringstream ss;
	ss << "Invalid AlgorithmID_t (" << algo_id << ") requested for AlgoData access!";
	throw larbys(ss.str());
      }
      if( index >= _data_id_map[algo_id].size() ) {
	std::stringstream ss;
	ss << "Invalid data index (" << index
	   << ") requested for AlgoData access for algorithm " << Name(algo_id)
	   << " (Algorithm ID " << algo_id << ")";
	throw larbys(ss.str());
      }
      return _data_id_map[algo_id][index];
    }

    const AlgoDataBase* AlgoDataManager::Data(const AlgorithmID_t algo_id, const Index_t index) const
    { return Data(AlgoDataID(algo_id,index)); }

    const AlgoDataBase* AlgoDataManager::Data(const AlgoDataID_t data_id) const
    {
      if(data_id >= _data_v.size()) {
	std::stringstream ss;
	ss << "Invalid AlgoDataID_t (" << data_id << ") requested" << std::endl;
	throw larbys(ss.str());
      }
      return _data_v[data_id];
    }

    void AlgoDataManager::AssociateMany(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b)
    {
      auto const id_a = obj_a.ID();
      auto const id_b = obj_b.ID();
      auto const index_a = obj_a.Index();
      auto const index_b = obj_b.Index();
      if(id_a >= _data_v.size()) {
	LAROCV_CRITICAL() << "Object A (ID=" << id_a << ") does not exist in data container..." << std::endl;
	throw larbys();
      }
      if(index_a == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Object A (ID=" << id_a << ") index is invalid!" << std::endl;
	throw larbys();
      }
      if(id_b >= _data_v.size()) {
	LAROCV_CRITICAL() << "Object B (ID=" << id_b << ") does not exist in data container..." << std::endl;
	throw larbys();
      }
      if(index_b == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Object B (ID=" << id_b << ") index is invalid!" << std::endl;
	throw larbys();
      }
      _ass_man.AssociateMany(obj_a,obj_b);
    }

    void AlgoDataManager::AssociateOne(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b)
    {
      auto const id_a = obj_a.ID();
      auto const id_b = obj_b.ID();
      auto const index_a = obj_a.Index();
      auto const index_b = obj_b.Index();
      if(id_a >= _data_v.size()) {
	LAROCV_CRITICAL() << "Object A (ID=" << id_a << ") does not exist in data container..." << std::endl;
	throw larbys();
      }
      if(index_a == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Object A (ID=" << id_a << ") index is invalid!" << std::endl;
	throw larbys();
      }
      if(id_b >= _data_v.size()) {
	LAROCV_CRITICAL() << "Object B (ID=" << id_b << ") does not exist in data container..." << std::endl;
	throw larbys();
      }
      if(index_b == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Object B (ID=" << id_b << ") index is invalid!" << std::endl;
	throw larbys();
      }
      _ass_man.AssociateOne(obj_a,obj_b);
    }

  }
}

#endif
