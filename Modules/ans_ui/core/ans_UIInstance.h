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
 
 There may be multiple UIInstances for the same UIModel. UIInstance serve for reference only
 and do not take ownwership of UIAdaptor/Component, which are owned by the component hierarchy.
 */

struct UIInstance :
            public std::enable_shared_from_this<UIInstance>
{
    UIInstance (UIModel* uiModel, UISpec* uiSpec, std::shared_ptr<UIInstance> parentInstance = nullptr, bool forMockup = false);
   ~UIInstance ();
    
    std::shared_ptr<UIInstance> getChildInstanceFor (UIModel* uiModel, UISpec* uiSpec);
    
    UIModel* getModel() { return model.get(); }
    UISpec*  getSpec()  { return spec.get(); }
    
    /** Return true, if this is a top-level UIInstance belonging to a WindowUIModel, rather than some EmbeddedUIModel */
    bool isForWindow();
    
    /** Add an adaptor (incl. its associated component) to the model's registry */
    bool registerAdaptor (UIAdaptor* adaptor);
    
    /**
     Remove an adaptor (incl. its associated component) from the model's registry.
     This is called when an adaptor is deleted, which happens when its Component gets
     deleted. You should NEVER call this yourself.
     */
    void unregisterAdaptor (UIAdaptor* adaptor);
    
    /**
     Removes all possibly remaining adaptors and deletes them, if they haven't been deleted
     and unregistered by the component hierarchy already. This is a final resort for cleanup
     before populating the same UIModel with the same UISpec again, or when shutting down a
     window (usually there should be no UIAdaptor left anyway).
     */
    void clear();
    
    /**
     Get a registered UIAdaptor by identifier.
     If there is no such adaptor, the instance will also look into its children.
     */
    UIAdaptor* getAdaptor (const ComponentID& identifier, bool lookIntoChildren = false);

    /**
     Get the component associated with an UIAdaptor that is registered with the identifier.
     If there is no such adaptor, the instance will also look into its children.
     Returns nullptr if no such component or adaptor is known.
     */
    Component* getComponent (const ComponentID& identifier, bool lookIntoChildren = false);
    
    /** Provide a fake instance for hosting mockup components used by UIEditor for drawing purposes only */
    std::shared_ptr<UIInstance> getMockupUI();
    
    /** Whether this instance is used for mockup widgets only */
    bool isMockup() { return mockup; }
    
    /** Re-deploy changes made to the UISpec regarding layout to all registered components (live editing) */
    void updateLayoutFromSpec();
    
    String printDebug();

protected:
    
    void childWasDeleted (UIInstance* child);
    
private:
    using Registry = HashMap<ComponentID,UIAdaptor*>;
    
    std::shared_ptr<UIInstance> findChildFor (UIModel* m, UISpec* s);
    void updateLayoutFromSpec (ComponentSpec* spec);
   
    std::weak_ptr<UIInstance> parent;
    WeakReference<UISpec> spec;
    WeakReference<UIModel> model;
    Array<std::shared_ptr<UIInstance>> children; // owned
    Registry registry;
    std::shared_ptr<UIInstance> mockupUI;
    bool mockup;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIInstance)
};

}
