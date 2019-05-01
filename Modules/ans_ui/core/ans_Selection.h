/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

//#include "JuceHeader.h"

namespace ans {
    using namespace juce;

/**
 Selection manages a multiple or single selection of objects, by keeping a set of indexes
 identifying those. It is the responsibility of its owner to map those indexes to actual
 objects kept in whatever container the owner chooses.
 
 Selection takes care of keeping a consistent state with respect to multi vs. single
 selections and whether an empty selection is allowed.
 */

class Selection
{
public:
    
    using Set = SortedSet<int>;
    
    Selection (bool multi = true, bool mayBeEmpty = true, int initial = -1) :
        enableMultipleSelection (multi),
        enableNullSelection (mayBeEmpty),
        changed (false)
    {
        if (initial >= 0)
            selected.add (initial);
    }
    
    virtual ~Selection () {}
    
    /** Return the number of currently selected indexes */
    int size () const { return selected.size(); }
    
    bool isEmpty() const { return size() == 0; }
    
    bool isNullSelectionEnabled() const  { return enableNullSelection; }
    bool isMultipleSelectionEnabled() const { return enableMultipleSelection; }
    
    /**
     Force-empty the current selection. This does NOT preserve a single selection,
     even if empty selections are not allowed!
     */
    void clear()
    {
        if (selected.isEmpty())
            return;
        selected.clear();
        changed = true;
    }
    
    /** Whether a given index is currently selected */
    bool contains (int index) const
    {
        return selected.contains (index);
    }
    
    /** Select or deselect a particular index incrementally */
    void setSelectionOf (int index, bool state)
    {
        if (state)
        {
            if (index < 0 || contains(index))
                return;
            
            if (!enableMultipleSelection && !isEmpty() && !contains(index))
                return set(index);
            
            selected.add (index);
        }
        else
        {
            if (!contains (index))
                return;
            
            if (!enableNullSelection && size() == 1)
                return;
            
            selected.remove (index);
        }
        changed = true;
    }
    
    /**
     Get the currently selected index, or -1 if nothing is selected. If multiple indexes
     are selected, this will return only the first one
     */
    int get() const
    {
        return selected.isEmpty() ? -1 : selected.getFirst();
    }
    
    /** Replace the current selection with a single-selection */
    void set (int index)
    {
        if (index < 0 || (contains(index) && size() == 1))
            return;
        
        selected.clear();
        selected.add (index);
        changed = true;
    }
    
    /** Get a copy of all currently selected indexes */
    Set getMulti() const
    {
        return selected;
    }

    /** Set multiple indexes in one go */
    void setMulti (const Set& indexes)
    {
        if (indexes == selected)
            return;
        
        selected = indexes;
        changed = true;
    }
    
    /** Remove all selected indexes >= max. This may end up with an empty selection, even if not allowed! */
    void constrain (int max)
    {
        Set copy = selected;
        for (auto i : copy)
            if (i >= max)
            {
                selected.remove(i);
                changed = true;
            }
    }
    
    /** Whether the selection has changed since the last call to hasChanged(). Every call resets this status */
    bool hasChanged()
    {
        bool answer = changed;
        changed = false;
        return answer;
    }
    
    /** For debugging */
    const String toString() const
    {
        String out;
        out << "Selection (";
        for (auto i : selected)
        {
            out << i;
            if (i != selected.getLast())
                out << ", ";
        }
        out << ")";
        return out;
    }
    
    /** This tests only for equal selection indexes, NOT whether both selections use the same constraints */
    bool operator== (const Selection& other) const
    {
        return selected == other.selected;
    }
    
    /** This tests only for equal selection indexes, NOT whether both selections use the same constraints */
    bool operator!= (const Selection& other) const
    {
        return selected != other.selected;
    }

protected:
    
    bool enableMultipleSelection;
    bool enableNullSelection;
    bool changed;
    Set selected;
};


/**
 Classes that want to keep a Selection can derive from SelectionBased to provide a common higher-level interface.
 */

class SelectionBased
{
public:
    SelectionBased (bool enableMultiSelection, bool enableEmptySelection) :
        selection (enableMultiSelection, enableEmptySelection)
    {}
    
    virtual ~SelectionBased () {}
    
    int             getSelectedIndex()                                          { return selection.get(); }
    void            setSelectedIndex (int i)                                    { selection.set(i); ensureValidSelection(); }
    Selection::Set  getMultiSelectionIndexes()                                  { return selection.getMulti(); }
    void            setMultiSelectionIndexes (const Selection::Set& indexes)    { selection.setMulti (indexes); ensureValidSelection(); }
    
    /** Get a reference to the selection */
    Selection& getSelection() { return selection; }
    
    /** Replace the current selection, e.g. as supplied by a data binding */
    void setSelection (const Selection& newSelection)
    {
        selection.setMulti (newSelection.getMulti());
        ensureValidSelection();
    }
    
protected:
    Selection selection;
    
private:
    /** Derived classes should implement, unless selection is unconstrained */
    virtual void ensureValidSelection() {}
};


/**
 UIItemInterface is a required interface for items kept in UIListModel and UITreeModel.
 With the UI framework evolving, this can be extended as needed.
 */

class UIItemInterface
{
public:
    virtual ~UIItemInterface() {}
    
    /** Provide a String for display in lists or trees */
    virtual const String getItemString() const = 0;
    
    /** Return true, if the item most likely (or definitely) has children (used by UITreeModel only) */
    virtual bool itemHasChildren() { return false; }
    
    /** Retrieve and return the item's children (used by UITreeModel only) */
    virtual Array<UIItemInterface*> getItemChildren() { return {}; };
    
    /** Provide a unique identifier for the item (used by UITreeModel only) */
    virtual const String getItemIdentifier() { return getItemString(); }

protected:
    
    /** Convert any array of objects into an array of UIItemInterface */
    template <typename ArrayClass>
    Array<UIItemInterface*> convert (const ArrayClass& children)
    {
        Array<UIItemInterface*> answer;
        for (auto each : children)
            answer.add (dynamic_cast<UIItemInterface*> (each));
        return answer;
    }
    
};


}
