/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "../core/ans_Binding.h"
#include "../core/ans_Selection.h"
#include "../specs/ans_ComponentSpec.h"

namespace ans {
    using namespace juce;

class UIModel;
class UIInstance;
class UIListModelBase;
class UITreeModelBase;

/**
 Any subclass of Component can derive from this class in order to add a communication
 channel with UIModel. UIAdaptor implements a uniform communication protocol that translates
 to and from an individual Juce Component's API.
 */

class UIAdaptor : public Dependent
{
public:

    UIAdaptor (UIInstance* instance, const ComponentSpec& spec);
    virtual ~UIAdaptor();
    
    //============================== GETTERS ===============================================
    
    /** Get content, state or attributes from the Component, overridden by type */
    virtual void getComponentState (const Binding::Purpose& p, Selection& selection) { warn(p); }

    /** Get content, state or attributes from the Component, overridden by type */
    virtual void getComponentState (const Binding::Purpose& p, var& value) { warn(p); }
    
    /** Get content, state or attributes from the Component, overridden by type */
    virtual void getComponentState (const Binding::Purpose& p, bool& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}

    /** Get content, state or attributes from the Component, overridden by type */
    virtual void getComponentState (const Binding::Purpose& p, int& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}

    /** Get content, state or attributes from the Component, overridden by type */
    virtual void getComponentState (const Binding::Purpose& p, double& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}
    
    /** Get content, state or attributes from the Component, overridden by type */
    virtual void getComponentState (const Binding::Purpose& p, String& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}
    
    
    //============================== SETTERS ===============================================
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, Selection& selection) { warn(p); }

    /**
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, UITreeModelBase& contents) { warn(p); }
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, UIListModelBase& contents) { warn(p); }
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, UIValueMap& contents) { warn(p); }

    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, const var& value)
    {
        if (p == Binding::Purpose::GetEnabled)
            return getComponent()->setEnabled (value);
        
        if (p == Binding::Purpose::GetVisible)
            return getComponent()->setVisible (value);
        
        warn(p);
    }
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, bool value)
    {
        if (p == Binding::Purpose::GetEnabled)
            return getComponent()->setEnabled (value);
        
        if (p == Binding::Purpose::GetVisible)
            return getComponent()->setVisible (value);
            
        var state = value;
        setComponentState (p, state);
    }
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, int value)
        { var state = value; setComponentState (p, state); }
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, double value)
        { var state = value; setComponentState (p, state); }
    
    /** Set a content, state or attribute of the component, overridden by type */
    virtual void setComponentState (const Binding::Purpose& p, const String& value)
        { var state = value; setComponentState (p, state); }
    
    
    //=====================================================================================
    
    /** Add a binding to the adaptor. Caution! The adaptor takes ownership of it! */
    void addBinding (Binding* binding);
    
    /** Triggers a binding for a particular purpose, if set */
    void performBinding (const Binding::Purpose& p) { performBindingIfSet (bindings[p]); };
    
    /** Check if a binding for a particular purpose is set */
    bool hasBinding (const Binding::Purpose& p) { return bindings[p] != nullptr; };
    
    //=====================================================================================
    
    virtual void componentBuildBegin() { insideBuild = true; }
    
    virtual void componentBuildEnd() { insideBuild = false; }
    
    /** Access the Component branch of the multi-inherited instance */
    Component* getComponent() { return dynamic_cast<Component*>(this); }
    
    /**
     Return a UIComposite suitable for being populated with content by UIBuilder,
     if the associated component supports that, else return nullptr. 
     */
    virtual UIComposite* getUIComposite() { return nullptr; };
    
    /** Get the UIInstance that owns the adaptor */
    UIInstance* getOwner();
    
    /** Get the UIModel of the UIInstance that owns the adaptor */
    UIModel* getModel();
    
    /** If this is a top-level adaptor for a window, it requires special treatment on deletion */
    virtual bool isWindowAdaptor() { return false; }

    //--------------------------------------------------------------------------------------------
    
    void update (Model* sender, Aspect aspect, void* argument) override;
    
    const ComponentID identifier;

friend class UIModel;
friend class UIInstance;
    
protected:
    
    void initialiseFromSpec (UIInstance* owner, const ComponentSpec& spec);
    void performBindingIfSet (Binding* binding);
    void warn (const Binding::Purpose& p);

    UIComponentClass::Type type;
    WeakReference<UIInstance> ui;
    Aspect defaultAspect;
    OwnedArray<Binding> bindings;
    bool insideUpdate;
    bool insideBuild;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAdaptor)
};

} // namespace

