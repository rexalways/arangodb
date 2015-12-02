////////////////////////////////////////////////////////////////////////////////
/// @brief collection key generator
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
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
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_VOC_BASE_KEY__GENERATOR_H
#define ARANGODB_VOC_BASE_KEY__GENERATOR_H 1

#include "Basics/Common.h"
#include "Basics/Mutex.h"

#include "VocBase/vocbase.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                    KEY GENERATORS
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                              FORWARD DECLARATIONS
// -----------------------------------------------------------------------------

struct TRI_json_t;

// -----------------------------------------------------------------------------
// --SECTION--                                                    public defines
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief maximum length of a key in a collection
////////////////////////////////////////////////////////////////////////////////

#define TRI_VOC_KEY_MAX_LENGTH (254)

// -----------------------------------------------------------------------------
// --SECTION--                                             GENERAL KEY GENERATOR
// -----------------------------------------------------------------------------

class KeyGenerator {

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief available key generators
////////////////////////////////////////////////////////////////////////////////

    enum GeneratorType {
      TYPE_UNKNOWN       = 0,
      TYPE_TRADITIONAL   = 1,
      TYPE_AUTOINCREMENT = 2
    };

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

  protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief create the generator
////////////////////////////////////////////////////////////////////////////////

    explicit KeyGenerator (bool);

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy the generator
////////////////////////////////////////////////////////////////////////////////

    virtual ~KeyGenerator ();

// -----------------------------------------------------------------------------
// --SECTION--                                           public static functions
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief initialize the lookup table for key checks
////////////////////////////////////////////////////////////////////////////////

    static void Initialize ();

////////////////////////////////////////////////////////////////////////////////
/// @brief get the generator type from JSON
////////////////////////////////////////////////////////////////////////////////

    static GeneratorType generatorType (struct TRI_json_t const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief create a key generator based on the options specified
////////////////////////////////////////////////////////////////////////////////

    static KeyGenerator* factory (struct TRI_json_t const*);

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief generate a key
////////////////////////////////////////////////////////////////////////////////

    virtual std::string generate (TRI_voc_tick_t) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a key
////////////////////////////////////////////////////////////////////////////////

    virtual int validate (std::string const&,
                          bool) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief track usage of a key
////////////////////////////////////////////////////////////////////////////////

    virtual void track (TRI_voc_key_t) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief return a JSON representation of the generator
////////////////////////////////////////////////////////////////////////////////

    virtual struct TRI_json_t* toJson (TRI_memory_zone_t*) const = 0;

// -----------------------------------------------------------------------------
// --SECTION--                                               protected functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief check global key attributes
////////////////////////////////////////////////////////////////////////////////

    int globalCheck (std::string const&,
                     bool);

// -----------------------------------------------------------------------------
// --SECTION--                                               protected variables
// -----------------------------------------------------------------------------

  protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the users can specify their own keys
////////////////////////////////////////////////////////////////////////////////

    bool _allowUserKeys;

////////////////////////////////////////////////////////////////////////////////
/// @brief lookup table for key checks
////////////////////////////////////////////////////////////////////////////////

    static std::array<bool, 256> LookupTable;

};

// -----------------------------------------------------------------------------
// --SECTION--                                         TRADITIONAL KEY GENERATOR
// -----------------------------------------------------------------------------

class TraditionalKeyGenerator : public KeyGenerator {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create the generator
////////////////////////////////////////////////////////////////////////////////

    explicit TraditionalKeyGenerator (bool);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy the generator
////////////////////////////////////////////////////////////////////////////////

    ~TraditionalKeyGenerator ();

// -----------------------------------------------------------------------------
// --SECTION--                                           public static functions
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a key
////////////////////////////////////////////////////////////////////////////////

    static bool validateKey (char const* key);

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief generate a key
////////////////////////////////////////////////////////////////////////////////

    std::string generate (TRI_voc_tick_t) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a key
////////////////////////////////////////////////////////////////////////////////

    int validate (std::string const&,
                  bool) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief track usage of a key
////////////////////////////////////////////////////////////////////////////////

    void track (TRI_voc_key_t) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief return the generator name
////////////////////////////////////////////////////////////////////////////////

    static std::string name () {
      return "traditional";
    }

////////////////////////////////////////////////////////////////////////////////
/// @brief return a JSON representation of the generator
////////////////////////////////////////////////////////////////////////////////

    struct TRI_json_t* toJson (TRI_memory_zone_t*) const override;
};

// -----------------------------------------------------------------------------
// --SECTION--                                      AUTO INCREMENT KEY GENERATOR
// -----------------------------------------------------------------------------

class AutoIncrementKeyGenerator : public KeyGenerator {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create the generator
////////////////////////////////////////////////////////////////////////////////

    AutoIncrementKeyGenerator (bool,
                               uint64_t,
                               uint64_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy the generator
////////////////////////////////////////////////////////////////////////////////

    ~AutoIncrementKeyGenerator ();

// -----------------------------------------------------------------------------
// --SECTION--                                           public static functions
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a key
////////////////////////////////////////////////////////////////////////////////

    static bool validateKey (char const* key);

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

  public:

////////////////////////////////////////////////////////////////////////////////
/// @brief generate a key
////////////////////////////////////////////////////////////////////////////////

    std::string generate (TRI_voc_tick_t) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a key
////////////////////////////////////////////////////////////////////////////////

    int validate (std::string const&,
                  bool) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief track usage of a key
////////////////////////////////////////////////////////////////////////////////

    void track (TRI_voc_key_t) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief return the generator name
////////////////////////////////////////////////////////////////////////////////

    static std::string name () {
      return "autoincrement";
    }

////////////////////////////////////////////////////////////////////////////////
/// @brief return a JSON representation of the generator
////////////////////////////////////////////////////////////////////////////////

    struct TRI_json_t* toJson (TRI_memory_zone_t*) const override;

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

  private:

    triagens::basics::Mutex _lock;

    uint64_t    _lastValue;     // last value assigned

    uint64_t    _offset;        // start value

    uint64_t    _increment;     // increment value
};

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a document id (collection name + / + document key)
////////////////////////////////////////////////////////////////////////////////

bool TRI_ValidateDocumentIdKeyGenerator (char const*,
                                         size_t*);

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
