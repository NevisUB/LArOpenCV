/**
 * \file AlgoDataBase.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class AlgoDataBase
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef ALGODATABASE_H
#define ALGODATABASE_H

#include "ImageClusterTypes.h"
#include <iostream>
namespace larocv {

  class AlgoDataManager;
  namespace data {
    class AlgoDataBase;
    class AlgoDataArrayElementBase;
    class AlgoDataArrayBase;

    /**
       \class AlgoDataBase
       @brief Base class for algorithm-specific data representation 
    */
    class AlgoDataBase {
      friend class AlgoDataManager;
      friend class AlgoDataArrayBase;
    public:
      
      /// Default constructor
      AlgoDataBase() : _id(kINVALID_SIZE), _category(kDataCatDefault), _type(kDataTypeUnknown)
      {}

      /// Default destructor
      virtual ~AlgoDataBase(){}
      
      /// Clear method
      virtual void Clear() { _Clear_(); }

      /// ID getter
      AlgoDataID_t ID() const
      { return _id; }

      /// Category getter
      AlgoDataCategory_t Category() const
      { return _category; }

      /// Type getter
      AlgoDataType_t Type() const
      { return _type; }

    protected:

      /// Clear method to be implemented by child
      virtual void _Clear_() {}
      
      /// Protected contructor, to be used by inherited class constructors
      AlgoDataBase(AlgoDataCategory_t cat, AlgoDataType_t type=kDataTypeUnknown)
	: _id(kINVALID_SIZE), _category(cat), _type(type)
      {}
      
    private:
      AlgoDataID_t       _id;
      AlgoDataCategory_t _category;
      AlgoDataType_t     _type;
    };

    /**
       \class AlgoDataElementBase
       @brief Base class for elements of array-like algorithm-specific data representation 
    */
    class AlgoDataArrayElementBase : public AlgoDataBase {
      friend class AlgoDataArrayBase;
    public:
      
      /// Default constructor
      AlgoDataArrayElementBase(AlgoDataType_t type=kDataTypeUnknown)
	: AlgoDataBase(kDataCatArrayElement,type)
	, _index(kINVALID_SIZE)
      {}
      
      /// Default destructor
      virtual ~AlgoDataArrayElementBase(){}
      
      /// Clear method
      void Clear() { AlgoDataBase::Clear(); _index = kINVALID_SIZE; }

      /// ID getter
      Index_t Index() const
      { return _index; }

    private:
      AlgoDataID_t _index;
    };


    /**
       \class AlgoDataArrayBase
       @brief Base class for an array-like algorithm-specific data representation 
    */
    class AlgoDataArrayBase : public AlgoDataBase {
      friend class AlgoDataManager;
    public:
      AlgoDataArrayBase(AlgoDataType_t type=kDataTypeUnknown)
	: AlgoDataBase(kDataCatArray,type)
      {}
      ~AlgoDataArrayBase(){}
    protected:
      void SetIndex(AlgoDataArrayElementBase& obj,Index_t index)
      { obj._index = index; }

      void SetID(AlgoDataArrayElementBase& obj, AlgoDataID_t id)
      { obj._id = id;}
    };
    
    /**
       \class AlgoDataArrayTemplate
       @brief Template class for an array-like algorithm-specific data representation 
    */
    template <class T>
    class AlgoDataArrayTemplate : public AlgoDataArrayBase {
      friend class AlgoDataManager;
      
    public:
      
      /// Default constructor
      AlgoDataArrayTemplate(AlgoDataType_t type=kDataTypeUnknown)
	: AlgoDataArrayBase(type)
      {}
      
      /// Default destructor
      virtual ~AlgoDataArrayTemplate(){}
      
      /// Clear method
      void Clear() { AlgoDataBase::Clear(); _data_v.clear(); }

      /// Creater
      T& create()
      {
	_data_v.resize(_data_v.size()+1);
	auto& target = _data_v.back();
	AlgoDataArrayBase::SetID(target,ID());
	AlgoDataArrayBase::SetIndex(target,(_data_v.size()-1));
	return target;
      }

      /// Registerer
      void push_back(const T& data)
      {
	_data_v.push_back(data);
	auto& target = _data_v.back();
	AlgoDataArrayBase::SetID(target,ID());
	AlgoDataArrayBase::SetIndex(target,(_data_v.size()-1));
      }

      /// Registerer
      void emplace_back(T&& data)
      {
	_data_v.emplace_back(std::move(data));
	auto& target = _data_v.back();
	AlgoDataArrayBase::SetID(target,ID());
	AlgoDataArrayBase::SetIndex(target,(_data_v.size()-1));
      }

      /// Const reference getter
      const std::vector<T>& as_vector() const
      { return _data_v; }

    private:
      std::vector<T> _data_v;
    };

    
  }
}

#endif
/** @} */ // end of doxygen group 

