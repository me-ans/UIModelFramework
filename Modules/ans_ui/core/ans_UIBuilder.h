/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

//#include "ans_UIModel.h"
//#include "ans_UIInstance.h"

namespace ans {
    using namespace juce;
    
/**
 UIBuilder does the heavy lifting of constructing a Component hierarchy from a UISpec and registering
 UIAdaptors with the associated UIModel. UIBuilder actually does what ComponentSpec is expected to do
 by itself, if this was really object-oriented. Due to circular header dependencies though, this
 construction work could not (yet) be moved to ComponentSpec and its subclasses.
 */
struct UIBuilder
{
    /** Build a new window according to UISpec */
    static std::unique_ptr<TopLevelWindow> buildWindow (UISpec* spec, WindowUIModel* model);
    
    /**
     Build the component hierarchy defined by the UISpec and install it into the composite,
     replacing its current contents. The resulting adaptors/components will be bound to
     the UIModel.
     */
    static bool buildInto (UISpec* spec, UIModel* model, UIComposite* composite);
    
    /**
     Build a UIComponentProxy for each of the components in the hierarchy defined by the UISpec
     and install them into the composite, replacing its current contents. The resulting adaptors/components
     will be bound to the UIModel, which should be a UIEditor.
     */
    static bool buildProxyInto (UISpec* spec, UIModel* model, UIComposite* composite, std::shared_ptr<UIInstance> mockups);
    

    static std::unique_ptr<Component> buildComponent (const ComponentSpec* spec,
                                                      std::shared_ptr<UIInstance> instance,
                                                      Component* parent);
    
    /** Build a UIComponentProxy for UIEditor the given spec */
    static std::unique_ptr<Component> buildProxy (const ComponentSpec* spec,
                                                  std::shared_ptr<UIInstance> instance,
                                                  Component* parent,
                                                  std::shared_ptr<UIInstance> mockups);
    
    /** Build an empty canvas, that is, clear it of all content */
    static bool buildEmptyCanvas (UIComposite* composite);
    
    /** Adds the 'edit' link to UIs (development builds only) */
    static void buildEditLink (std::shared_ptr<UIInstance> instance, UIComposite* composite);

};



/**
 UIComponentProxy acts as a proxy for an arbitrary component showing up in UIEditor.
 This class works very closely with UIEditor and its UITreeModel<ComponentSpec> and
 is of no use elsewhere.
 */

class UIComponentProxy :
        public juce::Component,
        public UIAdaptor
{
public:
    UIComponentProxy (std::shared_ptr<UIInstance> instance, const ComponentSpec& spec, std::shared_ptr<UIInstance> mockups);
   ~UIComponentProxy ();
        
    void setComponentState (const Binding::Purpose& p, Selection& selection) override;
    
    void resized() override;
    void paint (Graphics& g) override;
    
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;

private:
    void updateDummy();
    
    ComponentSpec* editedSpec;
    std::unique_ptr<Component> dummy;
    std::shared_ptr<UIInstance> mockupUI;
    bool selected;
};
    
}
