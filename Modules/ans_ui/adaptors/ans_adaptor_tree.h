/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_adaptor_base.h"
#include "../core/ans_UIModel.h"

namespace ans {
    using namespace juce;

class UITreeView;

/**
 Base class for differently typed tree items, serving the purpose as a common denominator for
 UIAdaptor API. An entire tree is owned by its root item, so if the root item is deleted,
 this will delete all children recursively.
 */

class UITreeItemBase : public TreeViewItem
{
public:
    UITreeItemBase (int ident) :
        TreeViewItem(),
        index (ident)
    {}
    
    ~UITreeItemBase()
    {}
    
    int getIndex() const { return index; }
    
    void itemSelectionChanged (bool nowSelected) override;
    
    /** Enumerate the immediate children, dynamically casted to the desired type */
    template <typename LAMBDA, typename ObjectClass = UITreeItemBase>
    void enumerateImmediateChildren (LAMBDA&& each)
    {
        for (int i = 0; i < getNumSubItems(); ++i)
            if (auto child = dynamic_cast<ObjectClass*>(getSubItem(i)))
                each (child);
    }

    /** Recursively enumerate all items in the hierarchy, starting from this item, dynamically casted to the desired type */
    template <typename LAMBDA, typename ObjectClass = UITreeItemBase>
    void enumerateTreeItems (LAMBDA&& each)
    {
        each (this);
        for (int i = 0; i < getNumSubItems(); ++i)
            if (auto child = dynamic_cast<ObjectClass*>(getSubItem(i)))
                child->enumerateTreeItems (each);
    }
    
protected:
    int index;
};

/**
 UITreeItem is a container for any object to be presented by UITreeModel. UITreeItem requires
 the object to understand UIItemInterface. UITreeItem does NOT take ownership of the object it
 represents, so you must keep it alive while the tree model is operating.  An entire tree is
 owned by its root UITreeItem, so if the root item is deleted, this will delete all children
 recursively. That's why UITreeItem does not own the objects it represents.
 */

template <typename ObjectClass>
class UITreeItem  : public UITreeItemBase
{
public:
    
    /** Construct from pointer, if getTreeChildren() provides pointers */
    UITreeItem (ObjectClass* o, int& ident) :
        UITreeItemBase (ident++),
        object (o)
    {
        for (auto child : object->getItemChildren())
            if (auto c = dynamic_cast<ObjectClass*>(child))
                addSubItem (new UITreeItem (c, ident));
            else {
                jassertfalse; }
    }
    
    /** Construct from reference, if getTreeChildren() provides references to objects */
    UITreeItem (ObjectClass& o, int& ident) :
        UITreeItemBase (ident++),
        object (&o)
    {
        for (auto child : object->getItemChildren())
            if (auto c = dynamic_cast<ObjectClass&>(child))
                addSubItem (new UITreeItem (c, ident));
            else {
                jassertfalse; }
    }
    
    String getUniqueName() const override { return object->getItemIdentifier(); }
    
    void paintItem (Graphics& g, int width, int height) override
    {
        if (isSelected())
            g.fillAll (Colours::lightblue);
        
        g.setColour (Colours::white);
        g.setFont (height * 0.7f);
        g.drawText (object->getItemString(), 4, 0, width - 4, height, Justification::centredLeft, true);
    }
    
    bool mightContainSubItems() override { return object->itemHasChildren(); }
    
    /** Get the object associated with the tree item */
    ObjectClass* getObject() { return object; }
    
    /** Search the tree downwards starting from this item and return the item that bears the given object, or nullptr if none exists */
    UITreeItem* findItemWithObject (ObjectClass* o)
    {
        if (object == o)
            return this;
        
        for (int i = 0; i < getNumSubItems(); ++i)
            if (auto found = dynamic_cast<UITreeItem*>(getSubItem(i))->findItemWithObject (o))
                return found;
        
        return nullptr;
    }
    
    /** Search the tree downwards starting from this item and return the item with the given index, or nullptr if none exists */
    UITreeItem* findItemWithIndex (int indexToLookFor)
    {
        if (index == indexToLookFor)
            return this;
        
        for (int i = 0; i < getNumSubItems(); ++i)
            if (auto found = dynamic_cast<UITreeItem*>(getSubItem(i))->findItemWithIndex (indexToLookFor))
                return found;
        
        return nullptr;
    }
    
private:
    ObjectClass* object;
};



/**
 UITreeModelBase is base class for differently templated UITreeModel<ObjectClass>
 so they can be passed along and stored uniformly.
 */

class UITreeModelBase : public SelectionBased
{
public:
    UITreeModelBase (bool enableMultiSelection, bool enableEmptySelection) :
        SelectionBased (enableMultiSelection, enableEmptySelection),
        views ()
    {}
    
    virtual ~UITreeModelBase () {}
    
    void addView (UITreeView* owner) { views.add (owner); }
    void removeView (UITreeView* owner) { views.remove (owner); }
    
    /** Get the item at the root */
    virtual UITreeItemBase* getRootItem() { return nullptr; }
    
protected:
    void selectionChanged ();
    virtual void ensureValidSelection() {}
    
    ListenerList<UITreeView> views;
};


/**
 UITreeModel keeps any type of object that implements UIItemInterface. It maintains a selection
 state that UIModel can use to access the currently selected objects. You can derive from this
 class for any objects to show up in a tree.
 */

template <typename ObjectClass>
class UITreeModel : public UITreeModelBase
{
public:
    
    using ItemType = UITreeItem<ObjectClass>;
    
    UITreeModel (bool enableMultiSelection = false, bool enableNullSelection = true, ObjectClass* initial = nullptr) :
        UITreeModelBase (enableMultiSelection, enableNullSelection),
        identCounter (0)
    {
        if (initial != nullptr)
            setSingleSelection (initial);
    }
    
   ~UITreeModel () {}
    
    
    /** Replace the entire tree with a new root object */
    void setRoot (ObjectClass* item)
    {
        identCounter = 0;
        if (item != nullptr)
            root = new ItemType (item, identCounter);
        else
            root = nullptr;
        ensureValidSelection();
    }
    
    /** Get a pointer to the object at the root of the tree */
    ObjectClass* getRoot()
    {
        if (root != nullptr)
            return root->getObject();
        else
            return nullptr;
    }
    
    UITreeItemBase* getRootItem() override { return root; }
    
    /** Get the unique index of an object in the tree, or -1 if no such object exists  */
    int getIndexOf (ObjectClass* o)
    {
        if (auto item  = root->findItemWithObject (o))
            return item->getIndex();
        else
            return -1;
    }
    
    /** Select or deselect a particular object incrementally */
    void setSelectionOf (ObjectClass* o, bool state)
    {
        selection.setSelectionOf (getIndexOf (o), state);
    }
    
    /** Check whether an object is included with the current selection */
    bool isSelected (ObjectClass* o)
    {
        return selection.contains (getIndexOf (o));
    }
    
    /**
     Get a pointer to the currently selected object. If multiple objects are selected, this will
     return only the first one.
     */
    ObjectClass* getSingleSelection()
    {
        if (root == nullptr)
            return nullptr;
        
        if (auto item  = root->findItemWithIndex (selection.get()))
            return item->getObject();
        else
            return nullptr;
    }
    
    /** Select one object in the tree */
    void setSingleSelection (ObjectClass* o)
    {
        if (root == nullptr || o == nullptr)
            selection.clear();
        else
        {
            if (auto item = root->findItemWithObject (o))
                selection.set (item->getIndex());
            else
            {   // You are selecting an unknown item
                jassertfalse;
            }
        }
        ensureValidSelection();
    }
    
    /** Get an array of pointers to the currently selected objects */
    Array<ObjectClass*> getMultipleSelection()
    {
        Array<ObjectClass*> answer;
        
        for (auto i : selection.getMulti())
            answer.add (root->findItemWithIndex (i));
        
        return answer;
    }
    
    /** Select one or more objects in the list */
    void setMultipleSelection (const Array<ObjectClass*>& objects)
    {
        Selection newSelection (selection.isMultipleSelectionEnabled(), selection.isNullSelectionEnabled());
        
        for (auto o : objects)
            if (auto item = root->findItemWithObject (o))
                newSelection.setSelectionOf (item->getIndex(), true);
        
        setSelection (newSelection);
    }
    
private:
    
    void ensureValidSelection() override
    {
        if (root == nullptr)
            return selection.clear();
        
        Selection::Set previous = selection.getMulti();
        for (auto index : previous)
            if (index < 0 || root->findItemWithIndex (index) == nullptr)
                selection.setSelectionOf (index, false);
        
        if (selection.get() == -1 && !selection.isNullSelectionEnabled())
            selection.set(0); // root
    }
    
    ScopedPointer<ItemType> root;
    int identCounter;
};

/**
 UITreeView extends its Juce equivalent with a communication channel to UIModel.
 */
class UITreeView :
        public juce::TreeView,
        public UIAdaptor
{
public:
    UITreeView (UIInstance* owner, const TreeSpec& spec);
   ~UITreeView ();
    
    void setComponentState (const Binding::Purpose& p, UITreeModelBase& value) override;    
    void getComponentState (const Binding::Purpose& p, Selection& value) override;
    void setComponentState (const Binding::Purpose& p, Selection& value) override;
    
    void itemSelectionChanged (UITreeItemBase* selectedItem, bool nowSelected);
    
private:
    template <typename LAMBDA>
    void enumerateTreeItems (LAMBDA&& each)
    {
        if (auto root = dynamic_cast<UITreeItemBase*> (getRootItem()))
            root->enumerateTreeItems (each);
    }
    
    UITreeModelBase* treeModel;
};



}

