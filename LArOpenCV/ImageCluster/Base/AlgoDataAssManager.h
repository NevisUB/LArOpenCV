/**
 * \file AlgoDataAssManager.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class AlgoDataAssManager
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef ALGODATAASSMANAGER_H
#define ALGODATAASSMANAGER_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "AlgoDataBase.h"
#include <TTree.h>
#include <vector>

namespace larocv {
  namespace data {

    /**
       \class AlgoDataAssManager
       @brief Manager for associations between AlgoDataBase inherit class objects
    */
    class AlgoDataAssManager : public laropencv_base {

    public:
      
      /// Default constructor
      AlgoDataAssManager() : laropencv_base("AlgoDataAssManager")
      {}
      
      /// Default destructor
      ~AlgoDataAssManager(){}

      /// Clear data method
      void ClearData();

      /// Check if the subject association is one-to-one 
      bool IsOneToOne(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const;

      /// Check if the subject association is many-to-many
      bool IsManyToMany(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const;

      /// Create one-to-one association
      void AssociateOne(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b);

      /// Create many-to-many association
      void AssociateMany(const AlgoDataArrayElementBase& obj_a, const AlgoDataArrayElementBase& obj_b);

      /// Association getter
      const std::vector<std::vector<Index_t> >&
      GetManyAss(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const;

      /// Association getter
      const std::vector<Index_t>&
      GetManyAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataID_t id_b) const;

      /// Association search + getter
      const std::vector<std::vector<Index_t> >&
      FindManyAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const;

      /// Association search + getter
      const std::vector<Index_t>&
      FindManyAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const;

      /// Association search + getter + assertion_on_uniqueness
      const std::vector<std::vector<Index_t> >&
      FindManyUniqueAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const;

      /// Association search + getter + assertion_on_uniqueness
      const std::vector<Index_t>&
      FindManyUniqueAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const;

      /// Association getter
      const std::vector<Index_t>&
      GetOneAss(const AlgoDataID_t id_a, const AlgoDataID_t id_b) const;

      /// Association getter
      Index_t
      GetOneAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataID_t id_b) const;

      /// Association search + getter
      const std::vector<Index_t>&
      FindOneAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const;

      /// Association search + getter
      Index_t
      FindOneAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const;

      /// Association search + getter + assertion_on_uniqueness
      const std::vector<Index_t>&
      FindOneUniqueAss(const AlgoDataID_t id_a, const AlgoDataType_t type_b) const;

      /// Association search + getter + assertion_on_uniqueness
      Index_t
      FindOneUniqueAss(const AlgoDataArrayElementBase& obj_a, const AlgoDataType_t type_b) const;


    private:
      std::vector< std::vector< std::vector< std::vector< Index_t > > > > _many_to_many;
      std::vector< std::vector< std::vector< Index_t > > > _one_to_one;
      std::vector<Index_t> _empty_ass;
      std::vector<std::vector<Index_t> > _empty_ass_v;
      std::vector<AlgoDataType_t> _type_v;
    };
  }
}
#endif
/** @} */ // end of doxygen group 

