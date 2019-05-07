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

class TabPageList;
    
/**
 Any subclass of Component can derive from this class in order to add a communication
 channel with UIModel. UIAdaptor implements a uniform communication protocol that translates
 to and from an individual Juce Component's API. Since UIAdaptor is tied to a Component and
 vice versa (sister classes), ultimate ownership of a UIAdaptor is with the Component.
 */

class UIAdaptor : public Dependent
{
public:
    
    UIAdaptor (std::shared_ptr<UIInstance> instance, const ComponentSpec& spec);
    virtual ~UIAdaptor();
    
    //============================== GETTERS ===============================================
    
    /**
     Get current (multi) selection state from the Component. Subclasses for components
     supporting selections must override this, unless it's for a simple component that uses
     a primitive type for selection, e.g. bool, int or String.
     */
    virtual void getComponentState (const Binding::Purpose& p, Selection& selection) { warn(p); }

    /**
     Get some content, state or attribute from the Component. This most general version uses var, which conveniently
     subsumes and accommodates multiple basic types and is therefore most convenient for subclasses to override and use.
     Implementors are responsible for distinguishing different purposes that use the same value type.
     */
    virtual void getComponentState (const Binding::Purpose& p, var& value) { warn(p); }
    
    /**
     Get a bool content, state or attribute from the Component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
    virtual void getComponentState (const Binding::Purpose& p, bool& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}

    /**
     Get an int content, state or attribute from the Component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
    virtual void getComponentState (const Binding::Purpose& p, int& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}

    /**
     Get a double content, state or attribute from the Component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
    virtual void getComponentState (const Binding::Purpose& p, double& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}
    
    /**
     Get a String content, state or attribute from the Component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use the
     same value type.
     */
    virtual void getComponentState (const Binding::Purpose& p, String& value)
        { var buffer;  getComponentState (p, buffer); value = buffer;}
    
    
    //============================== SETTERS ===============================================
    
    /**
     Set the current (multi) selection state of the component. Subclasses for components
     supporting selections must override this, unless it's for a simple component that uses
     a primitive type for selection, e.g. bool, int or String.
     */
    virtual void setComponentState (const Binding::Purpose& p, Selection& selection) { warn(p); }

    /** Supply the adaptor with a UITreeModel to work on */
    virtual void setComponentState (const Binding::Purpose& p, UITreeModelBase& contents) { warn(p); }
    
    /** Supply the adaptor with a UIListModel to work on */
    virtual void setComponentState (const Binding::Purpose& p, UIListModelBase& contents) { warn(p); }
    
    /** Supply the adaptor with a set of key->value pairs, e.g. for a menu */
    virtual void setComponentState (const Binding::Purpose& p, UIValueMap& contents) { warn(p); }
    
    /** Supply the adaptor with a TabPageList, e.g. for a tabbed component */
    virtual void setComponentState (const Binding::Purpose& p, std::shared_ptr<TabPageList> contents) { warn(p); }

    /**
     Set a var content, state or attribute of the component. This most general version uses var, which
     conveniently subsumes and accommodates multiple basic types and is therefore most convenient for
     subclasses to override and use. Implementors are responsible for distinguishing different purposes
     that use the same value type.
     */
    virtual void setComponentState (const Binding::Purpose& p, const var& value)
    {
        if (p == Binding::Purpose::GetEnabled)
            return getComponent()->setEnabled (value);
        
        if (p == Binding::Purpose::GetVisible)
            return getComponent()->setVisible (value);
        
        warn(p);
    }
    
    /**
     Set a bool content, state or attribute of the component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
    virtual void setComponentState (const Binding::Purpose& p, bool value)
    {
        if (p == Binding::Purpose::GetEnabled)
            return getComponent()->setEnabled (value);
        
        if (p == Binding::Purpose::GetVisible)
            return getComponent()->setVisible (value);
            
        var state = value;
        setComponentState (p, state);
    }
    
    /**
     Set a bool content, state or attribute of the component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
    virtual void setComponentState (const Binding::Purpose& p, int value)
        { var state = value; setComponentState (p, state); }
    
    /**
     Set a bool content, state or attribute of the component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
    virtual void setComponentState (const Binding::Purpose& p, double value)
        { var state = value; setComponentState (p, state); }
    
    /**
     Set a String content, state or attribute of the component. Falls back to the version using var, unless
     overridden by a subclass. Implementors are responsible for distinguishing different purposes that use
     the same value type.
     */
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
    
    /** Derived classes may do whatever is necessary when the contents of their component are rebuilt */
    virtual void componentBuildBegin() { insideBuild = true; }
    
    /** Derived classes may do whatever is necessary when the contents of their component are rebuilt */
    virtual void componentBuildEnd() { insideBuild = false; }
    
    /** Access the Component branch of the multi-inherited instance */
    Component* getComponent() { return dynamic_cast<Component*>(this); }
    
    /**
     Add a child component bounded by a given LayoutFrame. The adaptor takes ownership of it.
     Only composite adaptors support this, for others this issues a warning!
     */
    virtual void addComponent (std::unique_ptr<Component> comp);
    
    /**
     Add a child component bounded by a given LayoutFrame. The adaptor takes ownership of it.
     Only composite adaptors support this, for others this issues a warning!
     */
    void addComponent (std::unique_ptr<Component> comp, const LayoutFrame& frame);
    
    /**
     Return a UIComposite suitable for being populated with content by UIBuilder,
     if the associated component supports that, else return nullptr. 
     */
    virtual UIComposite* getUIComposite() { return nullptr; };
    
    /** Get the UIInstance that owns the adaptor */
    std::shared_ptr<UIInstance> getUIInstance();
    
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
    
    void initialiseFromSpec (std::shared_ptr<UIInstance> instance, const ComponentSpec& spec);
    void performBindingIfSet (Binding* binding);
    void warn (const Binding::Purpose& p);

    UIComponentClass::Type type;
    std::shared_ptr<UIInstance> ui;
    Aspect defaultAspect;
    OwnedArray<Binding> bindings;
    Array<std::unique_ptr<Component>> ownedComponents;
    bool insideUpdate;
    bool insideBuild;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIAdaptor)
};

} // namespace

