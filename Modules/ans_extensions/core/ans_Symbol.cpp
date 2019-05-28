/**
 Class Extensions For Juce
 -------------------------
 Copyright 2019 me-ans@GitHub
 Please regard the license terms accompanying this Juce module.
 */

#include "ans_Symbol.h"

namespace ans
{
    using namespace juce;
    

SymbolTable::SymbolTable () :
    lock(),
    table(),
    tableIndex()
{
    // Add the NULL Symbol
    table.add (SymbolTableEntry ("", 0));
    tableIndex.set ("", 0);
}

SymbolTable::~SymbolTable ()
{
    tableIndex.clear();
    table.clear();
}

bool SymbolTable::contains (SymbolID unique) const
{
    for (auto& each : table)
        if (each.unique == unique)
            return true;
    
    return false;
}

SymbolTableEntry& SymbolTable::lookup (SymbolID unique) const
{
    ScopedLock sl (lock);

    if (unique >= table.size())
    {
        print();
        jassertfalse;
    }
    return table.getReference(unique);
}

SymbolID SymbolTable::lookupOrCreateEntry (const String& name, bool incrementRefs)
{
    if (name.isEmpty())
        return 0;
    
    ScopedLock sl (lock);
    jassert (tableIndex.getNumSlots() > 0); // NULL Symbol must have been created already
    SymbolID unique = tableIndex[name];
    if (unique > 0)
    {
        if (incrementRefs)
            incrementReferences(unique);
        return unique;
    }
    
    SymbolTableEntry entry (name, table.size());
    table.add (entry);
    tableIndex.set (name, entry.unique);
    return entry.unique;
}

int SymbolTable::incrementReferences (SymbolID unique)
{
    ScopedLock sl (lock);
    
    if (table.isEmpty())
        return 0; // table already cleared
    
    SymbolTableEntry& entry = lookup(unique);
    entry.references++;
    return entry.references;
}

int SymbolTable::decrementReferences (SymbolID unique)
{
    ScopedLock sl (lock);
    
    if (table.isEmpty())
        return 0; // table already cleared

    SymbolTableEntry& entry = lookup(unique);
    entry.references--;
    /** @todo: Delete when no longer referenced, or do a garbage collection */
    return entry.references;
}

void SymbolTable::print() const
{
    ScopedLock sl (lock);
    
    DBG ("SymbolTable (" << table.size() << ")");
    for (auto entry : table)
    {
        DBG (entry.print());
        // Quick hack integrity test
        jassert (entry.unique == tableIndex[entry.name]);
    }
}

/****************************************************************************************/

String SymbolTableEntry::print () const
{
    String out;
    out << "[" << unique << "] " << name << " <" << references << ">";
    return out;
}
    
/****************************************************************************************/

Symbol::~Symbol()
{
    SymbolTable::instance().decrementReferences(unique);
}

Symbol::Symbol ()
{
    unique = 0;
    SymbolTable::instance().incrementReferences(unique);
}

Symbol::Symbol (const Symbol& other)
{
    unique = other.unique;
    SymbolTable::instance().incrementReferences(unique);
}

Symbol::Symbol (const String& nm)
{
    // A Symbol cannot be created from an empty string!
    jassert (nm.isNotEmpty());
    jassert (isValidIdentifier(nm));
    unique = SymbolTable::instance().lookupOrCreateEntry(nm);
}

Symbol::Symbol (const char* nm)
{
    // A Symbol cannot be created from an empty string!
    jassert (nm != nullptr && nm[0] != 0);
    jassert (isValidIdentifier(nm));
    unique = SymbolTable::instance().lookupOrCreateEntry(nm);
}

Symbol::Symbol (String::CharPointerType start, String::CharPointerType end)
{
    // A Symbol cannot be created from an empty string!
    jassert (start < end);
    String nm (start, end);
    jassert (isValidIdentifier(nm));
    unique = SymbolTable::instance().lookupOrCreateEntry (nm);
}

bool Symbol::isValidIdentifier (const String& possibleIdentifier) noexcept
{
    return possibleIdentifier.isNotEmpty()
        && possibleIdentifier.containsOnly ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-:#@$%");
}

SymbolTableEntry& Symbol::lookup () const
{
    return SymbolTable::instance().lookup (unique);
}

}

