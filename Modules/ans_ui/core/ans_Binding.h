/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "../utility/ans_SourceOutputStream.h"

namespace ans {
    using namespace juce;
    
    
class UIModel;
class ComponentSpec;
class UIAdaptor;
class UIComposite;

/** Disable to generate more verbose and redundant spec code */
#define USE_BINDING_MARCOS 1

/** Macro for member function access */
#define MEMBER_FN(prtToObject,ptrToMember)  (prtToObject->*ptrToMember)


/** ObjectAndSource pairs a copy-by-value object with its source code for code (re)generation */

template <typename Object>
struct ObjectAndSource
{
    ObjectAndSource (Object o, const String& s) : object(o), source(s) {}
    
    Object getObject() const { return object; }
    const String getSource() const { return source; }
    
private:
    Object object;
    const String source;
};

/** Macro to avoid redundant repetition and ensure proper spelling when initialising ObjectAndSource */
#define FROM_SOURCE(e) {e, #e}

/**
 Macro for passing an object paired with its source code as two separate arguments.
 This makes sense only in contexts where the two arguments are following each other!
 */
#define MEMBER(e) e, #e
#define SOURCE(e) e, #e


/**
 Binding is the base class of various type-specific getters, setters and action triggers that implement
 the communication between UIModel and UIAdaptor. Bindings call the model in order to update a component's
 contents, or to trigger some action with the model. When a Binding is performed, data exchange is handled
 by type-specific subclasses of Binding.
 
 This base class exists to allow for differently templated subclasses to reside in a uniform
 container and respond to a uniform protocol.
 */

class Binding
{
public:
    
    /**
     Binding::Purpose::Signature distinguishes low-level method signatures.
     For each Signature and value type there is a specific templated subclass of Binding.
     */
    typedef enum {
        Update_Component = 0,   ///< UIModel provides Component with updated contents or state
        Update_Model,           ///< Component provides UIModel with changed contents or state
        Trigger_Action,         ///< Component calls UIModel member function w/o arguments
        Pass_Component,         ///< Component calls UIModel member function with Component as argument
        Populate_Composite      ///< UIModel populates an empty UIComposite with content
    } Signature;
    
    /**
     Binding::Purpose defines the method signature and use of a data binding (communication channel),
     which UIAdaptor uses to broker between UIModel and various types of Component. For any
     UIAdaptor, a user interface can establish exactly one binding per type. Therefore the
     assorted types, which are globally available, make up the 'vocabulary' of communication between
     UIModel and Component.
     */
    struct Purpose
    {
        Purpose (int key_,
                 Signature signature_,
                 const String& name_,
                 bool userDefinedAspectRequired_,
                 bool respondToUndefined_ = false,
                 Aspect defaultAspect = Model::Undefined);
        
        /** Converts to int, so it can be used as an index into Array, etc. */
        operator int() const { return key; }
        
        bool operator== (const Purpose& p) const noexcept { return key == p.key; }
        
        /** Get global Type constant by name */
        static const Purpose parse (const String& n);
        
        /** Known purposes for use in source code (and for generating source code) */
        
        static const Purpose GetEnabled;       ///< Ask UIModel about enablement status of the component
        static const Purpose GetVisible;       ///< Ask UIModel about visibility status of the component
        static const Purpose GetValue;         ///< Get contents of the component from UIModel
        static const Purpose GetSelection;     ///< Get current selection state from UIModel and deploy to component
        static const Purpose GetLabel;         ///< Get label or title of the component from UIModel
        static const Purpose SetValue;         ///< Provide UIModel with component's current contents
        static const Purpose SetSelection;     ///< Provide UIModel with component's current selection state
        static const Purpose SetLabel;         ///< Provide UIModel with component's current label (e.g. selected tab)
        static const Purpose Action;           ///< Component calls UIModel member function w/o arguments
        static const Purpose Config;           ///< Calls UIModel with Component as argument, for the purpose of (re)configuring
        static const Purpose Layout;           ///< Calls UIModel with Component as argument, for the purpose of changing layout
        static const Purpose Canvas;           ///< Ask UIModel to populate an empty component shell with contents
        
        static Array<Binding::Purpose> KnownTypes;
        
        //Member Variables
        int key;
        Signature signature;
        String name;
        bool userDefinedAspectRequired;
        bool respondToUndefined;
        Aspect defaultAspect;
    };
    
    /** Base class constructor */
    Binding (const Purpose& p, const String& s, Aspect a = Model::Undefined) :
        purpose (p),
        source (s),
        aspect (a)
    {}
    
    virtual ~Binding () {}
    
    /** Whether an instance of UIModel is already set for this Binding */
    virtual bool isBound() const { return false; }
    
    /**
     Since Binding is originally created for and owned by ComponentSpec, it needs to be
     re-instantiated for every instance of it's model's class that it is used with.
     */
    virtual Binding* instantiateFor (UIModel* instance) = 0;
    
    /**
     Visitor pattern: Dispatches back to the adaptor with a method call appropriate
     for the specific callback's method signature and value type
     */
    virtual void performFor (UIAdaptor& adaptor) {}
        
    /** Whether a concrete subclass of Binding is triggered on the change of an Aspect */
    virtual bool isAspected() const { return true; }
    
    /**
     A Binding may be associated with an Aspect that makes it perform only selectively.
     Not all subclasses of Binding make use of it though (mostly getters only).
     Check isAspected() before using this.
     */
    virtual void setAspect (Aspect a) { aspect = a; }

    /**
     A Binding may be associated with an Aspect that makes it perform only selectively.
     Not all subclasses of Binding make use of it though (mostly getters)
     Check isAspected() before using this.
     */
    virtual Aspect getAspect () const { return aspect; }
    
    /**
     Render a C++ expression for use in spec files
     */
    virtual String generateSourceCPP (Model::Class* modelClass) const;
    
    /** Concrete subclasses must override: C++ expression template to fill-in */
    virtual const String generateSourceTemplate() const { return "Bind::$PURPOSE (MEMBER ($SOURCE)$ASPECT)"; }
    
    const Purpose& purpose;
    const String source;
    Aspect  aspect;
};


} // namespace ans



