/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "details/ans_LayoutSpecInspector.h"

namespace ans {
    using namespace juce;

class UIEditor;

/**
 Base class of all ComponentSpec inspectors. It operates on any generic ComponentSpec.
 Subclasses extend this default UI to handle the more specific specs.
 */

class ComponentSpecInspector   : public EmbeddedUIModel
{
public:
    
#include "ans_ComponentSpecInspector.specs.h"
    
    ComponentSpecInspector (UIModel* parent) : EmbeddedUIModel (parent), layoutInspector (this) {}
   ~ComponentSpecInspector () {}
    
    /** Get the parent UIEditor */
    UIEditor* getUIEditor();

    /** Get the UISpec being edited in the parent UIEditor */
    UISpec* getSelectedUISpec();
    
    /** Get the current spec that is being edited */
    ComponentSpec* getSelectedSpec () { return selectedSpec; }
    
    /** Replace the current spec to be edited */
    ComponentSpec* setSelectedSpec (ComponentSpec* specToEdit)
    {
        selectedSpec = specToEdit;
        changed ({ComponentSelection, ComponentSettings});
        return selectedSpec;
    }
    
    const ComponentID getSuggestedID (const ComponentID& base);
    
    
    String  getIdentifier () { return selectedSpec->identifier; }
    void    setIdentifier (const String& input) { selectedSpec->identifier = input; updateLayout(); }
    
    String  getLabel () { return selectedSpec->label; }
    void    setLabel (const String& input) { selectedSpec->label = input; updateLayout();  }
    
    String  getTooltip () { return selectedSpec->tooltip; }
    void    setTooltip (const String& input) { selectedSpec->tooltip = input; updateLayout();  }
    
    int     getFocusOrder () { return selectedSpec->focusOrder; }
    void    setFocusOrder (int input) { selectedSpec->focusOrder = input; updateLayout();  }
    
    /**
     Programmatically builds the UISpec for the inspector by populating a CompositeSpec
     with children. Each subclass extends the UI by overriding and calling
     Parent::populateInspector first. Note: composite is deliberately a pointer to allow
     for easy copy/paste of code snippets from other UISpec source code.
     */
    virtual void populateInspector (CompositeSpec* composite, LayoutCursor& cursor)
    {
        if (selectedSpec == nullptr)
            return;
        
        cursor.setCellSize ({0.33, 22});
        cursor.setSpacing ({0,1});
        
        makeLabel (composite,
                   cursor,
                   "title",
                   MEMBER (&ComponentSpecInspector::getInspectorTitle),
                   ComponentSelection);
        
        makeInput (composite,
                   cursor,
                   "inputId",
                   "Identifier",
                   MEMBER (&ComponentSpecInspector::getIdentifier),
                   MEMBER (&ComponentSpecInspector::setIdentifier),
                   ComponentSettings);
        
        makeInput (composite,
                   cursor,
                   "inputLabel",
                   "Label",
                   MEMBER (&ComponentSpecInspector::getLabel),
                   MEMBER (&ComponentSpecInspector::setLabel),
                   ComponentSettings);
        
        makeText  (composite,
                   cursor,
                   "inputTip",
                   "Tooltip",
                   MEMBER (&ComponentSpecInspector::getTooltip),
                   MEMBER (&ComponentSpecInspector::setTooltip),
                   ComponentSettings);
        
        makeInput (composite,
                   cursor,
                   "inputFocus",
                   "Focus",
                   MEMBER (&ComponentSpecInspector::getFocusOrder),
                   MEMBER (&ComponentSpecInspector::setFocusOrder),
                   ComponentSettings);
        
        cursor.newLine();
        auto layoutInspector = new CanvasSpec ("layoutInspector");
        layoutInspector->setLayout (cursor.nextRows(7));
        layoutInspector->addBinding (Bind::Canvas (MEMBER(&ComponentSpecInspector::populateLayoutCanvas), ComponentSelection));
        composite->addComponent (layoutInspector);
    }
    
    String getInspectorTitle () { return selectedSpec->getClass()->getUserLabel(); }
                                     
    void populateCanvas (UIComposite* canvas)
    {
        auto content = new CompositeSpec ("content");
        ScopedPointer<UISpec> ui = new UISpec (*getClass(), content);
        LayoutCursor cursor;
        populateInspector (content, cursor);
        populateComposite (canvas, ui);
    }
                                     
     void populateLayoutCanvas (UIComposite* canvas)
     {
         layoutInspector.populateComposite (canvas);
     }
   
    void updateLayout();
    
    bool hasUIEditorLink() override { return false; }
    
    
    //======================================================================================================================
    // NOTE: These could be hidden somewhere else for general use!
    
    /** Utility function for adding an input field w/label */
    void makeHeader (CompositeSpec* composite,
                     LayoutCursor& cursor,
                     const String& ident,
                     const String& title)
    {
        cursor.newLine();
        cursor.nextCell();
        
        auto label = new LabelSpec (ident);
        label->setLayout (cursor.nextCells(2));
        label->setLabel (title);
        composite->addComponent (label);
    }
    
    /** Utility function for adding an input field w/label */
    template <typename InspectorClass, typename GetterType>
    void makeLabel (CompositeSpec* composite,
                    LayoutCursor& cursor,
                    const String& ident,
                    GetterType  (InspectorClass::*getter)(),
                    const String& getterSource,
                    Aspect aspect)
    {
        cursor.newLine();
        cursor.nextCell();
        
        auto label = new LabelSpec (ident);
        label->setLayout (cursor.nextCells(2));
        label->addBinding (Bind::GetLabel (getter, getterSource, aspect));
        composite->addComponent (label);
    }

    /** Utility function for adding an input field w/label */
    template <typename InspectorClass, typename GetterType, typename SetterType>
    void makeInput (CompositeSpec* composite,
                    LayoutCursor& cursor,
                    const String& ident,
                    const String& title,
                    GetterType  (InspectorClass::*getter)(),    const String& getterSource,
                    void (InspectorClass::*setter)(SetterType), const String& setterSource,
                    Aspect aspect)
    {
        cursor.newLine();
        
        auto label = new LabelSpec (ident + "Label");
        label->setLayout (cursor.nextCell());
        label->setLabel (title);
        composite->addComponent (label);
        
        auto input = new InputSpec (ident);
        input->setLayout (cursor.nextCells(2));
        input->addBinding (Bind::GetValue (getter, getterSource, aspect));
        input->addBinding (Bind::SetValue (setter, setterSource));
        composite->addComponent (input);
    }
    
    /** Utility function for adding an input field w/label */
    template <typename InspectorClass, typename GetterType, typename SetterType>
    void makeText  (CompositeSpec* composite,
                    LayoutCursor& cursor,
                    const String& ident,
                    const String& title,
                    GetterType  (InspectorClass::*getter)(),    const String& getterSource,
                    void (InspectorClass::*setter)(SetterType), const String& setterSource,
                    Aspect aspect)
    {
        cursor.newLine();
        
        auto label = new LabelSpec (ident + "Label");
        label->setLayout (cursor.nextCell());
        label->setLabel (title);
        composite->addComponent (label);
        
        auto input = new TextSpec (ident);
        input->setLayout (cursor.nextCells(2).withFixedHeight (3 * cursor.getCellSize().y - 2));
        input->addBinding (Bind::GetValue (getter, getterSource, aspect));
        input->addBinding (Bind::SetValue (setter, setterSource));
        composite->addComponent (input);
        
        cursor.newLine();
        cursor.newLine();
    }
    
    /** Utility function for adding an input field w/label */
    template <typename InspectorClass, typename GetterType, typename SetterType>
    void makeCheck (CompositeSpec* composite,
                    LayoutCursor& cursor,
                    const String& ident,
                    const String& title,
                    GetterType  (InspectorClass::*getter)(),    const String& getterSource,
                    void (InspectorClass::*setter)(SetterType), const String& setterSource,
                    Aspect aspect)
    {
        cursor.newLine();
        cursor.nextCells(1);
        auto input = new ToggleSpec (ident);
        input->setLayout (cursor.nextCells(3));
        input->setLabel (title);
        input->addBinding (Bind::GetSelection (getter, getterSource, aspect));
        input->addBinding (Bind::SetSelection (setter, setterSource));
        composite->addComponent (input);
    }
    
private:
    
    ComponentSpec* selectedSpec = nullptr;
    LayoutSpecInspector layoutInspector;
};
    
}

