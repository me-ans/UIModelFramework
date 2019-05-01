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
 Binding is the base class of various type-specific getters, setters and action triggers that
 implement the communication between UIModel and UIAdaptor.
 */

class Binding
{
public:
    
    /**
     Binding::Purpose::Signature distinguishes low-level method signatures.
     For each Signature and value type there is a specific templated subclass of Binding.
     */
    typedef enum {
        Update_Component = 0,
        Update_Model,
        Trigger_Action,
        Pass_Component,
        Populate_Composite
    } Signature;
    
    /**
     Purpose defines the method signature and use of a data binding (communication channel)
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
        
        static const Purpose GetEnabled;
        static const Purpose GetVisible;
        static const Purpose GetValue;
        static const Purpose GetSelection;
        static const Purpose GetLabel;
        static const Purpose SetValue;
        static const Purpose SetSelection;
        static const Purpose SetLabel;
        static const Purpose Action;
        static const Purpose Config;
        static const Purpose Layout;
        static const Purpose Canvas;
        
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



