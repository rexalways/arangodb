////////////////////////////////////////////////////////////////////////////////
/// @brief IndexNode
///
/// @file 
///
/// DISCLAIMER
///
/// Copyright 2010-2014 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Michael Hackstein
/// @author Jan Steemann
/// @author Copyright 2014, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_AQL_INDEX_NODE_H
#define ARANGODB_AQL_INDEX_NODE_H 1

#include "Basics/Common.h"
#include "Aql/Ast.h"
#include "Aql/ExecutionNode.h"
#include "Aql/RangeInfo.h"
#include "Aql/Range.h"
#include "Aql/types.h"
#include "Aql/Variable.h"
#include "Basics/JsonHelper.h"
#include "VocBase/voc-types.h"
#include "VocBase/vocbase.h"

namespace triagens {
  namespace aql {
    struct Collection;
    class Condition;
    class ExecutionBlock;
    class ExecutionPlan;
    struct Index;

////////////////////////////////////////////////////////////////////////////////
/// @brief class IndexNode
////////////////////////////////////////////////////////////////////////////////

    class IndexNode : public ExecutionNode {
      
      friend class ExecutionBlock;
      friend class IndexBlock;

      public:

        IndexNode (ExecutionPlan* plan,
                   size_t id,
                   TRI_vocbase_t* vocbase, 
                   Collection const* collection,
                   Variable const* outVariable,
                   std::vector<Index const*> indexes,
                   Condition const* condition)
          : ExecutionNode(plan, id), 
            _vocbase(vocbase), 
            _collection(collection),
            _outVariable(outVariable),
            _indexes(indexes),
            _condition(condition) {
          
          TRI_ASSERT(_vocbase != nullptr);
          TRI_ASSERT(_collection != nullptr);
          TRI_ASSERT(_outVariable != nullptr);
          TRI_ASSERT(_condition != nullptr);
        }

        IndexNode (ExecutionPlan*, triagens::basics::Json const& base);

        ~IndexNode () {
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node
////////////////////////////////////////////////////////////////////////////////

        NodeType getType () const override final {
          return INDEX;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the database
////////////////////////////////////////////////////////////////////////////////
        
        TRI_vocbase_t* vocbase () const {
          return _vocbase;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the collection
////////////////////////////////////////////////////////////////////////////////

        Collection const* collection () const {
          return _collection;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return out variable
////////////////////////////////////////////////////////////////////////////////

        Variable const* outVariable () const {
          return _outVariable;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief export to JSON
////////////////////////////////////////////////////////////////////////////////

        void toJsonHelper (triagens::basics::Json&,
                           TRI_memory_zone_t*,
                           bool) const override final;

////////////////////////////////////////////////////////////////////////////////
/// @brief clone ExecutionNode recursively
////////////////////////////////////////////////////////////////////////////////

        ExecutionNode* clone (ExecutionPlan* plan,
                              bool withDependencies,
                              bool withProperties) const override final;

////////////////////////////////////////////////////////////////////////////////
/// @brief getVariablesSetHere
////////////////////////////////////////////////////////////////////////////////

        std::vector<Variable const*> getVariablesSetHere () const override final {
          return std::vector<Variable const*>{ _outVariable };
        }
        
////////////////////////////////////////////////////////////////////////////////
/// @brief getVariablesUsedHere, returning a vector
////////////////////////////////////////////////////////////////////////////////

        std::vector<Variable const*> getVariablesUsedHere () const override final;

////////////////////////////////////////////////////////////////////////////////
/// @brief getVariablesUsedHere, modifying the set in-place
////////////////////////////////////////////////////////////////////////////////

        void getVariablesUsedHere (std::unordered_set<Variable const*>& vars) const override final;

////////////////////////////////////////////////////////////////////////////////
/// @brief estimateCost
////////////////////////////////////////////////////////////////////////////////

        double estimateCost (size_t&) const override final;

////////////////////////////////////////////////////////////////////////////////
/// @brief getIndexes, hand out the indexes used
////////////////////////////////////////////////////////////////////////////////

        std::vector<Index const*> getIndexes () const {
          return _indexes;
        }

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief the database
////////////////////////////////////////////////////////////////////////////////

        TRI_vocbase_t* _vocbase;

////////////////////////////////////////////////////////////////////////////////
/// @brief collection
////////////////////////////////////////////////////////////////////////////////

        Collection const* _collection;

////////////////////////////////////////////////////////////////////////////////
/// @brief output variable
////////////////////////////////////////////////////////////////////////////////

        Variable const* _outVariable;

////////////////////////////////////////////////////////////////////////////////
/// @brief the index
////////////////////////////////////////////////////////////////////////////////

        std::vector<Index const*> _indexes;
                   
////////////////////////////////////////////////////////////////////////////////
/// @brief the index(es) condition
////////////////////////////////////////////////////////////////////////////////
                   
        Condition const* _condition;

    };

  }   // namespace triagens::aql
}  // namespace triagens

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:


