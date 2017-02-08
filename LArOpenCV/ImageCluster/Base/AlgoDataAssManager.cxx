#ifndef __ALGODATAASSMANAGER_H__
#define __ALGODATAASSMANAGER_H__

#include "AlgoDataAssManager.h"
#include "LArOpenCV/Core/larbys.h"
#include <sstream>

namespace larocv {
  namespace data {

    void AlgoDataAssManager::ClearData()
    {
      _many_to_many.clear();
      for(auto& per_type_a : _many_to_many) {
	for(auto& per_type_b : per_type_a) {
	  per_type_b.clear();
	}
      }
    }

    bool AlgoDataAssManager::IsOneToOne(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const
    {
      if(_one_to_one.size() <= id_a || _one_to_one[id_a].size() <= id_b || _one_to_one[id_a][id_b].empty())
	return false;
      return true;
    }

    bool AlgoDataAssManager::IsManyToMany(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const
    {
      if(_many_to_many.size() <= id_a || _many_to_many[id_a].size() <= id_b || _many_to_many[id_a][id_b].empty())
	return false;
      return true;
    }
    
    const std::vector<std::vector<Index_t> >&
    AlgoDataAssManager::GetManyAss(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const
    {
      if(IsOneToOne(id_a,id_b)) {
	LAROCV_CRITICAL() << "AlgoDataID_t " << id_a << " => " << id_b << " association is not many-to-many..." << std::endl;
	throw larbys();
      }
      if(_many_to_many.size() <= id_a || _many_to_many[id_a].size() <= id_b)
	return _empty_ass_v;
      return _many_to_many[id_a][id_b];
    }
    
    const std::vector<Index_t>&
    AlgoDataAssManager::GetManyAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataID_t id_b) const
    {
      auto const& ref = GetManyAss(obj_a.ID(),id_b);
      if(obj_a.Index() >= ref.size())
	return _empty_ass;
      return ref[obj_a.Index()];
    }

    const std::vector<Index_t>&
    AlgoDataAssManager::GetOneAss(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const
    {
      if(IsManyToMany(id_a,id_b)) {
	LAROCV_CRITICAL() << "AlgoDataID_t " << id_a << " => " << id_b << " association is not one_to_one..." << std::endl;
	throw larbys();
      }
      if(_one_to_one.size() <= id_a || _one_to_one[id_a].size() <= id_b)
	return _empty_ass;
      return _one_to_one[id_a][id_b];
    }
    
    Index_t
    AlgoDataAssManager::GetOneAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataID_t id_b) const
    {
      auto const& ref = GetOneAss(obj_a.ID(),id_b);
      if(obj_a.Index() >= ref.size())
	return kINVALID_SIZE;
      return ref[obj_a.Index()];
    }

    void AlgoDataAssManager::AssociateMany(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b)
    {
      auto const id_a = obj_a.ID();
      auto const id_b = obj_b.ID();
      if(id_a == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataBase inherit class object A has an invalid ID..." << std::endl;
	throw larbys();
      }
      if(id_b == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataBase inherit class object B has an invalid ID..." << std::endl;
	throw larbys();
      }
      if(IsOneToOne(id_a,id_b)) {
	LAROCV_CRITICAL() << "AlgoDataID_t " << id_a << " => " << id_b << " is already registered as one-to-one..." << std::endl;
	throw larbys();
      }

      auto const index_a = obj_a.Index();
      auto const index_b = obj_b.Index();
      if(index_a == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataArrayElementBase inherit class object A has an invalid Index ..." << std::endl;
	throw larbys();
      }
      if(index_b == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataArrayElementBase inherit class object B has an invalid Index ..." << std::endl;
	throw larbys();
      }
      
      // store A=>B
      if( _many_to_many.size()             <= id_a    ) _many_to_many.resize(id_a+1);
      if( _many_to_many[id_a].size()       <= id_b    ) _many_to_many[id_a].resize(id_b+1);
      if( _many_to_many[id_a][id_b].size() <= index_a ) _many_to_many[id_a][id_b].resize(index_a+1);
      auto& target_a2b = _many_to_many[id_a][id_b][index_a];
      bool found=false;
      for(auto const& v : target_a2b) {
	if(v == index_b) {
	  found=true;
	  break;
	}
      }
      if(!found) target_a2b.push_back(index_b);
      else found=false;
      
      // store B=>A
      if( _many_to_many.size()             <= id_b    ) _many_to_many.resize(id_b+1);
      if( _many_to_many[id_b].size()       <= id_a    ) _many_to_many[id_b].resize(id_a+1);
      if( _many_to_many[id_b][id_a].size() <= index_b ) _many_to_many[id_b][id_a].resize(index_b+1);
      auto& target_b2a = _many_to_many[id_b][id_a][index_b];
      found=false;
      for(auto const& v : target_b2a) {
	if(v == index_a) {
	  found=true;
	  break;
	}
      }
      if(!found) target_b2a.push_back(index_a);

      if(_type_v.size() <= id_a) _type_v.resize(id_a+1,kDataTypeUnknown);
      if(_type_v.size() <= id_b) _type_v.resize(id_b+1,kDataTypeUnknown);
      if(_type_v[id_a] == kDataTypeUnknown)
	_type_v[id_a] = obj_a.Type();
      else if(_type_v[id_a] != obj_a.Type()) {
	LAROCV_CRITICAL() << "AlgoDataID_t = " << id_a << " changed its type from " << _type_v[id_a] << " to " << obj_a.Type() << std::endl;
	throw larbys();
      }
      if(_type_v[id_b] == kDataTypeUnknown)
	_type_v[id_b] = obj_b.Type();
      else if(_type_v[id_b] != obj_b.Type()) {
	LAROCV_CRITICAL() << "AlgoDataID_t = " << id_b << " changed its type from " << _type_v[id_b] << " to " << obj_b.Type() << std::endl;
	throw larbys();
      }
    }

    void AlgoDataAssManager::AssociateOne(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b)
    {
      auto const id_a = obj_a.ID();
      auto const id_b = obj_b.ID();
      if(id_a == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataBase inherit class object A has an invalid ID..." << std::endl;
	throw larbys();
      }
      if(id_b == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataBase inherit class object B has an invalid ID..." << std::endl;
	throw larbys();
      }
      if(IsManyToMany(id_a,id_b)) {
	LAROCV_CRITICAL() << "AlgoDataID_t " << id_a << " => " << id_b << " is already registered as many-to-many..." << std::endl;
	throw larbys();
      }

      auto const index_a = obj_a.Index();
      auto const index_b = obj_b.Index();
      if(index_a == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataArrayElementBase inherit class object A has an invalid Index ..." << std::endl;
	throw larbys();
      }
      if(index_b == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "AlgoDataArrayElementBase inherit class object B has an invalid Index ..." << std::endl;
	throw larbys();
      }
      
      // store A=>B
      if( _one_to_one.size()             <= id_a    ) _one_to_one.resize(id_a+1);
      if( _one_to_one[id_a].size()       <= id_b    ) _one_to_one[id_a].resize(id_b+1);
      if( _one_to_one[id_a][id_b].size() <= index_a ) _one_to_one[id_a][id_b].resize(index_a+1,kINVALID_SIZE);
      _one_to_one[id_a][id_b][index_a] = index_b;
      
      // store B=>A
      if( _one_to_one.size()             <= id_b    ) _one_to_one.resize(id_b+1);
      if( _one_to_one[id_b].size()       <= id_a    ) _one_to_one[id_b].resize(id_a+1);
      if( _one_to_one[id_b][id_a].size() <= index_b ) _one_to_one[id_b][id_a].resize(index_b+1,kINVALID_SIZE);
      _one_to_one[id_b][id_a][index_b] = index_a;

      if(_type_v.size() <= id_a) _type_v.resize(id_a+1,kDataTypeUnknown);
      if(_type_v.size() <= id_b) _type_v.resize(id_b+1,kDataTypeUnknown);
      if(_type_v[id_a] == kDataTypeUnknown)
	_type_v[id_a] = obj_a.Type();
      else if(_type_v[id_a] != obj_a.Type()) {
	LAROCV_CRITICAL() << "AlgoDataID_t = " << id_a << " changed its type from " << _type_v[id_a] << " to " << obj_a.Type() << std::endl;
	throw larbys();
      }
      if(_type_v[id_b] == kDataTypeUnknown)
	_type_v[id_b] = obj_b.Type();
      else if(_type_v[id_b] != obj_b.Type()) {
	LAROCV_CRITICAL() << "AlgoDataID_t = " << id_b << " changed its type from " << _type_v[id_b] << " to " << obj_b.Type() << std::endl;
	throw larbys();
      }
    }
    
    const std::vector<std::vector<Index_t> >&
    AlgoDataAssManager::FindManyAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const
    {
      if(_many_to_many.size() <= id_a) return _empty_ass_v;
      auto const& ass_a_v = _many_to_many[id_a];
      for(size_t i=0; i<ass_a_v.size(); ++i) {
	if(ass_a_v[i].empty() || _type_v[i] != type_b) continue;
	if(IsOneToOne(id_a,i)) continue;
	return ass_a_v[i];
      }
      return _empty_ass_v;
    }

    const std::vector<Index_t>&
    AlgoDataAssManager::FindManyAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const
    {
      auto const& ass = FindManyAss(obj_a.ID(),type_b);
      if(ass.size() <= obj_a.Index()) return _empty_ass;
      return ass[obj_a.Index()];
    }

    const std::vector<Index_t>&
    AlgoDataAssManager::FindOneAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const
    {
      if(_one_to_one.size() <= id_a) return _empty_ass;
      auto const& ass_a_v = _one_to_one[id_a];
      for(size_t i=0; i<ass_a_v.size(); ++i) {
	if(ass_a_v[i].empty() || _type_v[i] != type_b) continue;
	if(IsManyToMany(id_a,i)) continue;
	return ass_a_v[i];
      }
      return _empty_ass;
    }

    Index_t
    AlgoDataAssManager::FindOneAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const
    {
      auto const& ass = FindOneAss(obj_a.ID(),type_b);
      if(ass.size() <= obj_a.Index()) return kINVALID_SIZE;
      return ass[obj_a.Index()];
    }

    const std::vector<std::vector<Index_t> >&
    AlgoDataAssManager::FindManyUniqueAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const
    {
      if(_many_to_many.size() <= id_a) return _empty_ass_v;
      auto const& ass_a_v = _many_to_many[id_a];
      size_t target_index=kINVALID_SIZE;
      for(size_t i=0; i<ass_a_v.size(); ++i) {
	if(ass_a_v[i].empty() || _type_v[i] != type_b) continue;
	if(IsOneToOne(id_a,i)) continue;
	if(target_index != kINVALID_SIZE) {
	  LAROCV_CRITICAL() << "Associations from AlgoDataID_t = " << id_a << " to AlgoDataType_t = " << type_b
			    << "could not be uniquely determined!" << std::endl;
	  throw larbys();
	}
	target_index = i;
      }
      return (target_index == kINVALID_SIZE ? _empty_ass_v : ass_a_v[target_index]);
    }

    const std::vector<Index_t>&
    AlgoDataAssManager::FindManyUniqueAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const
    {
      auto const& ass = FindManyUniqueAss(obj_a.ID(),type_b);
      if(ass.size() <= obj_a.Index()) return _empty_ass;
      return ass[obj_a.Index()];
    }

    const std::vector<Index_t>&
    AlgoDataAssManager::FindOneUniqueAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const
    {
      if(_one_to_one.size() <= id_a) return _empty_ass;
      auto const& ass_a_v = _one_to_one[id_a];
      size_t target_index=kINVALID_SIZE;
      for(size_t i=0; i<ass_a_v.size(); ++i) {
	if(ass_a_v[i].empty() || _type_v[i] != type_b) continue;
	if(IsManyToMany(id_a,i)) continue;
	if(target_index != kINVALID_SIZE) {
	  LAROCV_CRITICAL() << "Associations from AlgoDataID_t = " << id_a << " to AlgoDataType_t = " << type_b
			    << "could not be uniquely determined!" << std::endl;
	  throw larbys();
	}
	target_index = i;
      }
      return (target_index == kINVALID_SIZE ? _empty_ass : ass_a_v[target_index]);
    }

    Index_t
    AlgoDataAssManager::FindOneUniqueAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const
    {
      auto const& ass = FindOneUniqueAss(obj_a.ID(),type_b);
      if(ass.size() <= obj_a.Index()) return kINVALID_SIZE;
      return ass[obj_a.Index()];
    }
  }
}
#endif
