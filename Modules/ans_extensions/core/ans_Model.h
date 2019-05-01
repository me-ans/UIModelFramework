/**
 Class Extensions For Juce
 -------------------------
 Copyright 2019 me-ans@GitHub
 Please regard the license terms accompanying this Juce module.
 */

#pragma once

namespace ans
{
    using namespace juce;

/**
 Aspects are symbolic representations of (partial) states of some object. They are
 typically used as an argument to change propagation messages that notify dependent
 objects (in Juce terminology: Listeners) which partial state in particular has changed,
 so they can respond selectively. An aspect is a unique Symbol, which is most efficiently
 denoted by the Symbol's unique ID
 */

typedef SymbolID Aspect;


/**
 Aspects is a set of zero or more Aspect, implemented as an array of their unique IDs.
 
 @todo: Sorting is fine for uniqueness, but destroys ordering which may be deliberately choosen!
 */

class Aspects
{
public:
    using Super = SortedSet<Aspect>;
    
    Aspects ();
    Aspects (Aspect a);
    Aspects (std::initializer_list<Aspect> list);
    
    /** Print all Symbols in the set */
    String toString() const;
    
    void add (Aspect a) { contents.add (a); }
    
    inline Aspect* begin() { return contents.begin(); }
    inline Aspect* end()   { return contents.end(); }
    
    SortedSet<Aspect> contents;
};



class Model;

/**
 Dependent is somewhat an equivalent to ChangeListener, only for Model acting as the broadcaster.
 The Model-Dependent idiom originates from the MVC paradigm.
 */

class Dependent
{
public:
    
    virtual ~Dependent() = default;
    
    /**
     Your subclass of Dependent must implement this method to receive
     change notifications from its Model.
     @param sender The Model that triggered the binding.
     @param aspect The aspect that changed with the Model.
     @param argument An optional generic parameter.
     */
    virtual void update (Model* sender, Aspect aspect, void* argument) = 0;
    
};


/**
 Model notifies any number of dependent objects about aspects that have changed with the Model.
 The difference to ChangeBroadcaster is that notifications carry arguments (aspects) that allow
 to distinguish between different aspects to which dependents can respond selectively.
 he Model-Dependent idiom originates from the MVC paradigm.
 
 @todo: Add support for asynchronous updates on the message thread (currently all synchronous)
 
 */

class Model
{
public:
    
    Model() noexcept;
    virtual ~Model();
    
    /** Default undefined aspect, which also serves as a wildcard */
    DEFINE_SYMBOL (Undefined);
    

    //-------------------------------------------------------------------------
    METACLASS_ROOT_BEGIN (Model)
    
    /**
     Get the names of aspects introduced by this class. These must
     match the corresponding variables defined in the class scope.
     */
    virtual Array<Aspect> getAspects() const { return { Undefined }; }
    
    /** Recursively collects all aspects from the base class 'Model' down to this class */
    virtual Array<Aspect> getAllAspects() const;
    
    /** Get the names of all aspects introduced by this class */
    Array<String> getAspectNames() const;
       
    /** Print a single aspect's name */
    virtual String printAspect (Aspect a) const;
    
    /** Return the Aspect enum associated with the given name */
    virtual Aspect parseAspect (const String& s, bool warn = true) const;
    
    /** Whether UIEditor may modify aspects, etc */
    virtual bool isEditable() { return false; }
    
    /** Answer the namespace name this model is defined in  */
    virtual const String getNameSpace() { return "ans"; }
    
    METACLASS_ROOT_END
    //-------------------------------------------------------------------------
    
    /**
     Registers a dependent to receive change bindings from this model.
     Trying to add a dependent that's already on the list will have no effect.
     */
    void addDependent (Dependent* dependent);
    
    /**
     Unregisters a dependent from the list.
     If the dependent isn't on the list, this won't have any effect.
     */
    void removeDependent (Dependent* dependent);
    
    /** Removes all dependents from the list. */
    void removeAllDependents();
    
    
    /**
     Causes a changed() message to be sent for all aspects in the given
     array and in the given order.
     */
    void changed (const Array<Aspect>& aspects);
    
    
    /** A particular Aspect has changed. This will broadcast an update message to all dependents */
    void changed (Aspect aspect, void* parameter = nullptr);
    
    /**
     Causes a changed() message to be sent for all aspects in the given
     set in the order of the enum.
     */
    void changedAspects (const Aspects& aspects);
    
private:

    ListenerList <Dependent> dependents;
    
    void callDependents (Aspect aspect, void* argument);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Model)
};

}
