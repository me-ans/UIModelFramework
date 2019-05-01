/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */


#pragma once

//#include "../specs/ans_UISpec.h"

namespace ans {
    using namespace juce;

class UIModel;

/**
 UIInstance keeps track of UIAdaptor/Component that have been created for a particular UIModel
 and UISpec. Its main purpose is to lookup UIAdaptor/Component by identifier, so interested
 UIModel can access and modify them in their postBuild() method, for example.
 */

struct UIInstance : public ReferenceCountedObject
{
    using Ptr = ReferenceCountedObjectPtr<UIInstance>;
    
    UIInstance (UIModel* uiModel, UISpec* uiSpec, UIInstance* parentInstance = nullptr, bool forMockup = false);
   ~UIInstance ();
    
    UIInstance* getChildInstanceFor (UIModel* uiModel, UISpec* uiSpec);
    
    UIModel* getModel() { return model.get(); }
    UISpec*  getSpec()  { return spec.get(); }
    
    /** Return true, if this is a top-level UIInstance belonging to a WindowUIModel, rather than some EmbeddedUIModel */
    bool isForWindow();
    
    bool registerAdaptor (UIAdaptor* adaptor);
    
    void unregisterAdaptor (UIAdaptor* adaptor);
    
    void clear();
    
    /**
     Get a registered UIAdaptor by identifier.
     If there is no such adaptor, the instance will also look into its children.
     */
    UIAdaptor* getAdaptor (const ComponentID& identifier, bool lookIntoChildren = false);

    Component* getComponent (const ComponentID& identifier, bool lookIntoChildren = false);
    
    /** Provide a fake instance for hosting mockup components used by UIEditor for drawing purposes only */
    UIInstance* getMockupUI();
    
    /** Whether this instance is used for mockup widgets only */
    bool isMockup() { return mockup; }
    
    void updateLayoutFromSpec();
    
    String printDebug();

protected:
    
    void childWasDeleted (UIInstance* child);
    
private:
    using Registry = HashMap<ComponentID,UIAdaptor*>;
    
    UIInstance* findChildFor (UIModel* m, UISpec* s);
    void updateLayoutFromSpec (ComponentSpec* spec);
   
    WeakReference<UIInstance> parent;
    WeakReference<UISpec> spec;
    WeakReference<UIModel> model;
    OwnedArray<UIInstance> children;
    Registry registry;
    UIInstance::Ptr mockupUI;
    bool mockup;
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (UIInstance)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIInstance)
};

}
