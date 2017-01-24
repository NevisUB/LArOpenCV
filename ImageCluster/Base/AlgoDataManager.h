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

#include "Core/laropencv_base.h"
#include "AlgoDataBase.h"
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
      
      /// Registration of algorithm data storage ttree (optional)
      void Register(TTree* tree);

      /// Clear data method
      void ClearData();
      
      /// Algorithm ID getter from name (slow search)
      AlgorithmID_t ID (const std::string& name) const;

      /// Algorithm Name getter from ID
      const std::string& Name (const AlgorithmID_t algo_id) const;
      
      /// Data access method
      const AlgoDataBase* Data(const AlgorithmID_t algo_id, const AlgorithmDataID_t data_id=0) const;
      
    private:
      bool   _tree_attached;
      TTree* _tree;
      std::vector<std::vector<data::AlgoDataBase*> > _data_vv;
      std::vector<std::string> _name_v;
    };
  }
}
#endif
/** @} */ // end of doxygen group 

