////////////////////////////////////////////////////////////////////////////////
/// @brief AQL IndexBlock
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
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Michael Hackstein
/// @author Copyright 2015, ArangoDB GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_AQL_INDEX_BLOCK_H
#define ARANGODB_AQL_INDEX_BLOCK_H 1

#include "Aql/Collection.h"
#include "Aql/ExecutionBlock.h"
#include "Aql/ExecutionNode.h"
#include "Aql/IndexNode.h"
#include "Aql/RangeInfo.h"
#include "Utils/AqlTransaction.h"
#include "VocBase/shaped-json.h"

struct TRI_doc_mptr_copy_t;
struct TRI_edge_index_iterator_t;
struct TRI_hash_index_element_multi_s;

namespace triagens {
  namespace aql {

    class AqlItemBlock;

    class ExecutionEngine;

// -----------------------------------------------------------------------------
// --SECTION--                                                   IndexRangeBlock
// -----------------------------------------------------------------------------

    class IndexBlock : public ExecutionBlock {

      public:

        IndexBlock (ExecutionEngine* engine,
                         IndexNode const* ep);

        ~IndexBlock ();

////////////////////////////////////////////////////////////////////////////////
/// @brief initialize, here we fetch all docs from the database
////////////////////////////////////////////////////////////////////////////////

        int initialize () override;

////////////////////////////////////////////////////////////////////////////////
/// @brief initializeCursor, here we release our docs from this collection
////////////////////////////////////////////////////////////////////////////////

        int initializeCursor (AqlItemBlock* items, size_t pos) override;

        AqlItemBlock* getSome (size_t atLeast, size_t atMost) override final;

////////////////////////////////////////////////////////////////////////////////
// skip between atLeast and atMost, returns the number actually skipped . . .
// will only return less than atLeast if there aren't atLeast many
// things to skip overall.
////////////////////////////////////////////////////////////////////////////////

        size_t skipSome (size_t atLeast, size_t atMost) override final;

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------
      
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief Forwards _iterator to the next available index
////////////////////////////////////////////////////////////////////////////////

        void startNextIterator ();

////////////////////////////////////////////////////////////////////////////////
/// @brief Initializes the indexes
////////////////////////////////////////////////////////////////////////////////

        bool initIndexes ();

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not one of the bounds expressions requires V8
////////////////////////////////////////////////////////////////////////////////

        bool hasV8Expression () const;

////////////////////////////////////////////////////////////////////////////////
/// @brief build the bounds expressions
////////////////////////////////////////////////////////////////////////////////

        void buildExpressions ();

////////////////////////////////////////////////////////////////////////////////
/// @brief free _condition if it belongs to us
////////////////////////////////////////////////////////////////////////////////

        void freeCondition ();

////////////////////////////////////////////////////////////////////////////////
/// @brief continue fetching of documents
////////////////////////////////////////////////////////////////////////////////

        bool readIndex (size_t atMost);

////////////////////////////////////////////////////////////////////////////////
// @brief: sorts the index range conditions and resets _posInRanges to 0
////////////////////////////////////////////////////////////////////////////////

        void sortConditions ();

////////////////////////////////////////////////////////////////////////////////
/// @brief andCombineRangeInfoVecs: combine the arguments into a single vector,
/// by intersecting every pair of range infos and inserting them in the returned
/// value if the intersection is valid. 
////////////////////////////////////////////////////////////////////////////////

        std::vector<RangeInfo> andCombineRangeInfoVecs (std::vector<RangeInfo> const&, 
                                                        std::vector<RangeInfo> const&) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief cartesian: form the cartesian product of the inner vectors. This is
/// required in case a dynamic bound evaluates to a list, then we have an 
/// "and" condition containing an "or" condition, which we must then distribute. 
////////////////////////////////////////////////////////////////////////////////

        IndexOrCondition* cartesian (std::vector<std::vector<RangeInfo>> const&) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief: subclass for comparing IndexAndConditions in _condition. Similar to
/// OurLessThan in the SortBlock
////////////////////////////////////////////////////////////////////////////////

        class SortFunc {
          public:
            SortFunc (std::vector<std::vector<size_t>> const& prefix, 
                      IndexOrCondition* condition,
                      bool reverse)
              : _prefix(prefix),
                _condition(condition), 
                _reverse(reverse) {
            }

            bool operator() (size_t const&,
                             size_t const&) const;

          private:
            std::vector<std::vector<size_t>> const& _prefix;
            IndexOrCondition* _condition;
            bool const _reverse;
        };

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief collection
////////////////////////////////////////////////////////////////////////////////

        Collection const* _collection;

////////////////////////////////////////////////////////////////////////////////
/// @brief document buffer
////////////////////////////////////////////////////////////////////////////////

        std::vector<TRI_doc_mptr_copy_t> _documents;

////////////////////////////////////////////////////////////////////////////////
/// @brief current position in _allDocs
////////////////////////////////////////////////////////////////////////////////

        size_t _posInDocs;

////////////////////////////////////////////////////////////////////////////////
/// @brief current position in _indexes
////////////////////////////////////////////////////////////////////////////////

        size_t _currentIndex;

////////////////////////////////////////////////////////////////////////////////
/// @brief _indexes holds all Indexes used in this block
////////////////////////////////////////////////////////////////////////////////

        std::vector<Index const*> _indexes;

////////////////////////////////////////////////////////////////////////////////
/// @brief _allBoundsConstant, this indicates whether all given bounds
/// are constant
////////////////////////////////////////////////////////////////////////////////

        std::vector<bool> _allBoundsConstant;
        bool _anyBoundVariable;

////////////////////////////////////////////////////////////////////////////////
/// @brief _allVariableBoundExpressions, list of all variable bound expressions
////////////////////////////////////////////////////////////////////////////////
        
        std::vector<Expression*> _allVariableBoundExpressions;

////////////////////////////////////////////////////////////////////////////////
/// @brief _inVars, a vector containing for each expression above
/// a vector of Variable*, used to execute the expression
/////////////////////////////////////////////////////////////////////////////////
        
        std::vector<std::vector<Variable const*>> _inVars;

////////////////////////////////////////////////////////////////////////////////
/// @brief _inRegs, a vector containing for each expression above
/// a vector of RegisterId, used to execute the expression
////////////////////////////////////////////////////////////////////////////////
        
        std::vector<std::vector<RegisterId>> _inRegs;

////////////////////////////////////////////////////////////////////////////////
/// @brief _iterator: holds the index iterator found using
/// getIndexIterator (if any) so that it can be read in chunks and not
/// necessarily all at once.
////////////////////////////////////////////////////////////////////////////////

        triagens::arango::IndexIterator* _iterator;

////////////////////////////////////////////////////////////////////////////////
/// @brief _condition: holds the complete condition this Block can serve for
////////////////////////////////////////////////////////////////////////////////
        
        AstNode const* _condition;
        
////////////////////////////////////////////////////////////////////////////////
/// @brief _condition: holds the complete condition this Block can serve for
////////////////////////////////////////////////////////////////////////////////
        
        AstNode* _evaluatedCondition;
        
////////////////////////////////////////////////////////////////////////////////
/// @brief _flag: since readIndex for primary, hash, edges indexes reads the
/// whole index, this is <true> if initIndex has been called but readIndex has
/// not been called, otherwise it is <false> to avoid rereading the entire index
/// with successive calls to readIndex.
//////////////////////////////////////////////////////////////////////////////////

        bool _flag;
        size_t _posInRanges;
        std::vector<size_t> _sortCoords;

////////////////////////////////////////////////////////////////////////////////
/// @brief _freeCondition: whether or not the _condition is owned by the
/// IndexRangeBlock and must be freed
////////////////////////////////////////////////////////////////////////////////

        bool _freeCondition;

        bool _hasV8Expression;

    };

  }  // namespace triagens::aql
}  // namespace triagens

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:

