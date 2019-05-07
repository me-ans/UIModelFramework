/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_adaptor_base.h"
#include "../core/ans_Selection.h"

namespace ans {
    using namespace juce;

/**
 Since String doesn't understand toString(), for example, this extra hoop seems necessary to allow
 for a variety of objects to print as a String
 */

namespace PrintHelpers
{
    template <typename Type> static const String print (const Type& o)                      { return o.getItemString(); };
    template <>                     const String print (const String& o)                    { return o; };
};


class UIListModelBase;


/**
 UIListBox extends ListBox for communication with UIModel.
 UIModel is responsible for providing a UIListModel this adaptor operates on.
 */

class UIListBox :
        public juce::ListBox,
        public UIAdaptor
{
public:
    UIListBox (std::shared_ptr<UIInstance> instance, const ListSpec& spec);
   ~UIListBox ();
    
    void setComponentState (const Binding::Purpose& p, UIListModelBase& contents) override;
    void getComponentState (const Binding::Purpose& p, Selection& selection) override;
    void setComponentState (const Binding::Purpose& p, Selection& selection) override;
    
    void someViewChangedSelection();
    
private:
    Selection currentSelection();
    
    WeakReference<UIListModelBase> listModel;
};


/**
 UIListModelBase is a base class for differently templated UIListModel<T> to be passed along and stored uniformly.
 */

class UIListModelBase :
    public SelectionBased,
    public juce::ListBoxModel
{
public:
    UIListModelBase (bool enableMultiSelection, bool enableEmptySelection) :
        SelectionBased (enableMultiSelection, enableEmptySelection),
        views ()
    {}
    
    virtual ~UIListModelBase () { masterReference.clear(); }
    
    void addView (UIListBox* owner) { views.add (owner); }
    void removeView (UIListBox* owner) { views.remove (owner); }
    
    void selectedRowsChanged (int lastRowSelected) override
    {
        views.call ([](auto& view) { view.someViewChangedSelection(); });
    }
    
protected:
    ListenerList<UIListBox> views;
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (UIListModelBase)
};


/**
 UIListModel extends ListBoxModel for communication with UIModel and the adaptor UIListBox. It maintains
 a selection state that UIModel can use to select/unselect objects. You can configure this class for
 more elaborate and non-trivial objects.
 
 UIListModel merely handles pointers to objects owned by someone else. You are responsible for keeping
 these objects alive as long as this model exists. For simple use cases, e.g. a list of strings or fixed
 collection of choices, you can supply it with physical data that is kept by the model.
 */

template <typename ObjectClass>
class UIListModel : public UIListModelBase
{
public:
    
    using ListType = Array<ObjectClass*>;
    using DataType = OwnedArray<ObjectClass>;
    
    UIListModel (bool enableMultiSelection = false, bool enableEmptySelection = true) :
        UIListModelBase (enableMultiSelection, enableEmptySelection),
        pointers (),
        data ()
    {}
    
   ~UIListModel () {}
    
    /**
     Initialise or replace all list items with pointers to objects. UIListModel does not take
     ownership of these, so you are responsible for keeping them alive during the lifespan of
     this model!
     */
    void setList (const Array<ObjectClass*>& items)
    {
        data.clear();
        pointers.clear();
        pointers.addArray (items);
        ensureValidSelection();
    }
    
    /**
     Make UIListModel reference an external array of physical copy-by-value objects.
     Your are responsible for keeping the external array alive during the lifespan of this model!
     */
    void setListReference (Array<ObjectClass>& items)
    {
        data.clear();
        pointers.clear();
        for (auto& each : items)
            pointers.add (&each);
        ensureValidSelection();
    }
    
    /**
     Make UIListModel maintain an array of physical objects so you need not to do this.
     This is best for simple immediate lists, e.g. a collection of choices represented as strings,
     which you don't want to maintain outside this model. ObjectClass must have a copy constructor.
     */
    void setListData (const Array<ObjectClass>& items)
    {
        data.clear();
        pointers.clear();
        for (auto& each : items)
        {
            ObjectClass* o = new ObjectClass(each);
            pointers.add (o);
            data.add (o);
        }
        ensureValidSelection();
    }
    
    /** Get a reference to the array of pointers to your objects */
    ListType& getList()  { return pointers; }
    
    /**
     Get a reference to the physical list data. This will return an empty array,
     if you did not supply physical list data to the model!
     */
    DataType& getListData()  { return data; }
    
    /** Query selection state of a particular object */
    bool isSelected (ObjectClass* item)
    {
        return selection.contains (pointers.indexOf(item));
    }
    
    /** Select or deselect a particular object incrementally */
    void setSelectionOf (ObjectClass* item, bool state)
    {
        selection.setSelectionOf (pointers.indexOf(item), state);
    }
    
    /**
     Get a pointer to the currently selected object. If multiple objects are selected, this will
     return only the first one.
     */
    ObjectClass* getSingleSelection()
    {
        auto index = selection.get();
        if (index < 0)
            return nullptr;
        jassert (index < pointers.size());
        return pointers[index];
    }
    
    /** Select one object in the list */
    void setSingleSelection (ObjectClass* item)
    {
        if (item == nullptr)
            selection.clear();
        else
        {
            jassert (pointers.indexOf (item) >= 0);
            selection.set (pointers.indexOf (item));
        }
        ensureValidSelection();
    }
    
    /** Get an array of pointers to the currently selected objects */
    Array<ObjectClass*> getMultipleSelection()
    {
        Array<ObjectClass*> answer;
        for (auto i : getMultiSelectionIndexes())
            answer.add (pointers[i]);
        return answer;
    }
    
    /** Select one or more objects in the list */
    void setMultipleSelection (const Array<ObjectClass*>& items)
    {
        Selection newSelection (selection.isMultipleSelectionEnabled(), selection.isNullSelectionEnabled());
        for (auto item : items)
            newSelection.setSelectionOf (pointers.indexOf (item), true);
        setSelection (newSelection);
    }
    
    int  getNumRows() override { return pointers.size(); }
    
    // void listBoxItemClicked (int row, const MouseEvent &) override;
    // void listBoxItemDoubleClicked (int row, const MouseEvent &) override;
    // void backgroundClicked (const MouseEvent &) override;
    
    void paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override
    {
        if (rowNumber >= pointers.size())
            return;
        
        if (rowIsSelected)
            g.fillAll (LookAndFeel::getDefaultLookAndFeel().findColour (TextEditor::highlightColourId));
        
        g.setColour (LookAndFeel::getDefaultLookAndFeel().findColour (ListBox::textColourId));
        Font f (height * 0.7f);
        f.setHorizontalScale (1.0f);
        g.setFont (f);
        g.drawText (PrintHelpers::print (*pointers[rowNumber]),
                    4, 0, width - 6, height,
                    Justification::centredLeft, true);
    }
    
    //void deleteKeyPressed (int lastRowSelected) override;
    //void returnKeyPressed (int lastRowSelected) override;
    
    String print()
    {
        String out;
        for (auto each : pointers)
            out << PrintHelpers::print (*each) << " ";
        return out;
    }
    
private:
    
    void ensureValidSelection() override
    {
        selection.constrain (pointers.size());

        if (selection.isEmpty() && !selection.isNullSelectionEnabled())
        {
            // Your selection may not be empty ?
            jassert (!pointers.isEmpty());
            selection.set(0);
        }
    }
    
    ListType  pointers;
    DataType  data; // optional only
};

}
