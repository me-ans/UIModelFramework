/**
 Class Extensions For Juce
 -------------------------
 Copyright 2019 me-ans@GitHub
 Please regard the license terms accompanying this Juce module.
 */

#pragma once

/** Use this macro to define Symbols as a global variables in header files */
#define DEFINE_SYMBOL(e) static Symbol e;

/** Use this macro to initialise global Symbols in cpp files */
#define INIT_SYMBOL(c,e) Symbol c::e = Symbol(#e);

/**
 Symbol generates a default hash for its name on creation only once. Subsequently the default
 hash is wrapped with modulo around whatever bounds are required when the Symbol is hashed,
 e.g. in a HashMap. This upper limit for the default hash should be high enough to accommodate
 the size of your biggest containers.
 */
#define SymbolHashUpperLimit 16384*32

namespace ans
{
using namespace juce;

/** Defines which primitive type to use as a Symbol's unique ID */
typedef int SymbolID;

/** SymbolTableEntry is a utilty class for SymbolTable (@see SymbolTable, Symbol) */

class SymbolTableEntry final
{
public:

    /** Creates the default entry for the null Symbol */
    SymbolTableEntry () :
        unique (0),
        name (""),
        hash (0),
        references (1)
    {}
    
    /** Creates a new entry (that must not have existed in the symbol table before) */
    SymbolTableEntry (const String& literal, SymbolID identifier) :
        unique (identifier),
        name (literal),
        references (1)
    {
        hash = DefaultHashFunctions::generateHash (literal, SymbolHashUpperLimit);
    }
    
    ~SymbolTableEntry() noexcept {}

    /** Used for sorting the SymbolTable */
    inline bool operator<  (const SymbolTableEntry& other) const noexcept { return name <  other.name; }
        
    /** The entry of the null symbol */
    static SymbolTableEntry null;
        
    /** For debugging */
    String print() const;

friend class Symbol;
friend class SymbolTable;
protected:
        
    SymbolID     unique;
    const String name;
    int          hash;
    int          references;
        
    JUCE_LEAK_DETECTOR (SymbolTableEntry)
};
    

/** SymbolTable is the global registry for all Symbols (@see Symbol) */
        
class SymbolTable final
{
public:
    
    SymbolTable();
   ~SymbolTable();
    
    /** Access the global SymbolTable singleton */
    static SymbolTable& instance() { static SymbolTable st; return st; }
    
    /** For debugging */
    void print() const;

friend class Symbol;
protected:
    /** Lookup a SymbolTableEntry by unique SymbolID */
    SymbolTableEntry& lookup (SymbolID unique) const;
    
    bool contains (SymbolID unique) const;
    
    /** Add a new entry and assign it a unique SymbolID */
    SymbolID lookupOrCreateEntry (const String& literal, bool incrementReferences = true);
    
    int incrementReferences (SymbolID unique);
    int decrementReferences (SymbolID unique);
    
    CriticalSection& getLock() { return lock; }

private:
    CriticalSection          lock;
    Array<SymbolTableEntry>  table;
    HashMap<String,SymbolID> tableIndex;
};
    
       
/**
 A Symbol is a globally unique identifier that associates a name with a unique integer.
 Comparing and hashing symbols is as fast as comparing and hasning integers. All known
 Symbols are registered in a global SymbolTable. They basically behave like integers with
 an associated name that can be looked up for printing and parsing.
 
 Using Symbols as global variables, you practically have a project-wide global enum
 that manages the mapping of names to unique integers (and vice versa) automatically.
 These variables look and feel like constants in your source code, behave like integers
 and greatly improve readability and maintainability.
 
 Symbol has a few benefits over juce::Identifier: Symbols behave like integers, making
 them more efficient for use in Array, SortedSet, HashMap, etc. Symbols pre-hash their
 name only once, making subsequent hashes of that integral hash much faster.
 */
        
class Symbol final
{
public:
    
    /** Creates the NULL Symbol, which is interpreted as 'undefined' */
    Symbol ();
    
    /** Creates a new Symbol, incrementing the reference count of the SymbolTableEntry with that name */
    Symbol (const char* name);
    
    /** Creates a new Symbol, incrementing the reference count of the SymbolTableEntry with that name */
    Symbol (const String& name);
    
    /** Creates a new Symbol, incrementing the reference count of the SymbolTableEntry with that name */
    Symbol (String::CharPointerType nameStart, String::CharPointerType nameEnd);
    
    /** Restores a Symbol from is unique ID. Obviously, this fails if the ID is yet unknown! */
    Symbol (SymbolID ident) : unique (ident) { jassert (SymbolTable::instance().contains (ident)); }

    /** Since Symbol is a reference to a SymbolTableEntry, copying a Symbol merely copies that reference */
    Symbol (Symbol&& other);
    
    /** Since Symbol is a reference to a SymbolTableEntry, copying a Symbol merely copies that reference */
    Symbol (const Symbol& other);
    
    /** Silently convert Symbol to int, so it mostly behaves like an integer */
    operator int() const { return unique; }
    
    /** Silently convert const String&, so it mostly behaves like an String where expected */
    operator const String&() const { return toString(); }

    /** Deleting a Symbol decrements the reference count of the SymbolTableEntry with its name */
    ~Symbol();
    
    /** Looks up the SymbolTableEntry associated with the Symbol */
    SymbolTableEntry& lookup () const;

    /** Compares two symbols by looking at their unique integer ID (fast) */
    inline bool operator== (const Symbol& other) const noexcept         { return unique == other.unique; }
    
    /** Compares two symbols by looking at their unique integer ID (fast) */
    inline bool operator!= (const Symbol& other) const noexcept         { return unique != other.unique; }

    /** Compares a Symbol with a String. This should be used rarely, if at all */
    inline bool operator== (StringRef other) const                      { return lookup().name == other; }
    inline bool operator!= (StringRef other) const                      { return lookup().name != other; }
    inline bool operator<  (StringRef other) const                      { return lookup().name <  other; }
    inline bool operator<= (StringRef other) const                      { return lookup().name <= other; }
    inline bool operator>  (StringRef other) const                      { return lookup().name >  other; }
    inline bool operator>= (StringRef other) const                      { return lookup().name >= other; }
    
    /** Compares the names of two Symbols, e.g. for sorting */
    inline bool operator<  (const Symbol& other) const                  { return lookup().name <  other.lookup().name; }
    inline bool operator<= (const Symbol& other) const                  { return lookup().name <= other.lookup().name; }
    inline bool operator>  (const Symbol& other) const                  { return lookup().name >  other.lookup().name; }
    inline bool operator>= (const Symbol& other) const                  { return lookup().name >= other.lookup().name; }

    /** Returns this symbol as a string, which is useful for printing symbols. */
    const String& toString() const                                      { return lookup().name; }

    /** Returns this symbol's raw string pointer. */
    operator String::CharPointerType() const                            { return lookup().name.getCharPointer(); }

    /** Returns this symbol's raw string pointer. */
    String::CharPointerType getCharPointer() const                      { return lookup().name.getCharPointer(); }

    /** Returns this symbol as a StringRef. */
    operator StringRef() const                                          { return lookup().name; }
    
    /** Returns a pre-cached hash of the symbol's name, which is a fast lookup */
    int hash() const                                                    { return lookup().hash; }

    /** Returns true if this Symbol is not null */
    bool isValid() const noexcept                                       { return unique != 0; }

    /** Returns true if this Symbol is null */
    bool isNull() const noexcept                                        { return unique == 0; }

    /** Checks a given string for characters that might not be valid in an SymbolTableEntry.
        Since Identifiers are used as a script variables and XML attributes, they should only contain
        alphanumeric characters, underscores, or the '-' and ':' characters.
    */
    static bool isValidIdentifier (const String& possibleIdentifier) noexcept;
    
    JUCE_LEAK_DETECTOR (Symbol)
    
private:
    SymbolID unique;
};
            
} // namespace ans

