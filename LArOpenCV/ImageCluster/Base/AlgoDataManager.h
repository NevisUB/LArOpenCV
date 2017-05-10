/**
 * \file AlgoDataManager.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class AlgoDataManager
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef ALGODATAMANAGER_H
#define ALGODATAMANAGER_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "AlgoDataBase.h"
#include "AlgoDataAssManager.h"
#include <TTree.h>
#include <vector>

namespace larocv {
  namespace data {

    /**
       \class AlgoDataManager
       @brief Manager for an algorithm specific data representation
    */
    class AlgoDataManager : public laropencv_base {

    public:
      
      /// Default constructor
      AlgoDataManager();
      
      /// Default destructor
      ~AlgoDataManager(){}

      /// Registration of a new algorithm
      void Register(const std::string name);
      
      /// Registration of algorithm existence
      void Register(const AlgorithmID_t algo_id, const std::string name);

      /// Registration of algorithm specific data
      void Register(const AlgorithmID_t id, AlgoDataBase* data);
      
      /// Registration of algorithm data storate ttree (optional)
      void Register(TTree* tree, std::vector<AlgorithmID_t> store_algo_id_v);
	
      /// Registration of algorithm data storage ttree (optional)
      void Register(TTree* tree);

      /// Clear data method
      void ClearData();

      /// Get a list of algorithm names
      const std::vector<std::string>& Names() const;
      
      /// Algorithm ID getter from name (slow search)
      AlgorithmID_t ID (const std::string& name) const;

      /// Algorithm Name getter from ID
      const std::string& Name (const AlgorithmID_t algo_id) const;
      
      /// AlgoDataID_t search method
      AlgoDataID_t AlgoDataID(const AlgorithmID_t algo_id, const Index_t index) const;

      /// Data access method
      const AlgoDataBase* Data(const AlgorithmID_t algo_id, const Index_t index) const;

      /// Data access method
      const AlgoDataBase* Data(const AlgoDataID_t data_id) const;

      /// Association storage method to create one-to-one association
      void AssociateOne(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b);

      /// Association storage method to create many-to-many association
      void AssociateMany(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b);

      /// Access to const ref of association manager
      const AlgoDataAssManager& AssManager() const { return _ass_man; }

      /// Public RSEE
      uint _run;
      uint _subrun;
      uint _event;
      uint _entry;

    private:
      bool   _tree_attached;
      TTree* _tree;
      std::vector<std::vector<AlgoDataID_t> > _data_id_map;
      std::vector<data::AlgoDataBase*> _data_v;
      std::vector<std::string> _name_v;
      AlgoDataAssManager _ass_man;
      
    };
  }
}
#endif
/** @} */ // end of doxygen group 

