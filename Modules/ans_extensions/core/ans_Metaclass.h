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
 Metaclass adds support for 'static virtual methods' and basic reflection on the class hierarchy.
 Properly used class-side inheritance helps with the organization and maintenance of large
 domain-specific class hierarchies. The Metaclass hierarchy is limited to single-inheritance.
 That is, each Metaclass can have only one superclass, which is however sufficiently expressive
 for most applications.
 
 @todo: getClass() should return a reference, as should all uses of persistently static relationships
 */

class Metaclass
{
public:
    Metaclass (const String& className, Metaclass* parent) :
        name (className),
        superClass (parent)
    {
        if (parent != nullptr)
            parent->subClasses.addIfNotAlreadyThere (this);
    }
    
    virtual ~Metaclass()
    {}
    
    static CriticalSection& sharedLock()
    {
        static CriticalSection lock;
        return lock;
    }
    /** Return the name of the class represented by this Metaclass */
    const String& getName() const { return name; }
    
    /** Return the parent of this Metaclass, or nullptr for root */
    Metaclass* getSuperClass() const { return superClass; }
    
    /** Get the immediate children of this Metaclass */
    Array<Metaclass*>& getSubClasses() { return subClasses; }
    
    /** Recursively enumerates this Metaclass and all its children in hierarchical order */
    template <typename Class, typename LAMBDA>
    void withAllSubClassesDo (LAMBDA&& each)
    {
        each (dynamic_cast<Class*> (this));
        for (auto child : subClasses)
            child->withAllSubClassesDo<Class> (each);
    }
    
    /** Recursively search all subclasses by name, incl. the receiver */
    template <typename Class>
    Class* findSubClassByName (const String& className)
    {
        Class* found = nullptr;
        withAllSubClassesDo<Class> ([&](Class* child)
        {
            if (child->getName() == className)
                found = dynamic_cast<Class*> (child);
        });
        return found;
    }
    
    /** Return the name of the class represented by this Metaclass */
    const String toString() { return getName(); }

private:
    const String name;
    Metaclass* superClass;
    Array<Metaclass*> subClasses;
};

} // namespace ans


/**
 Boilerplate that starts the definition of the top-most Metaclass in your hierarchy,
 which implements "static virtual" methods for the class defined in the current scope.
 */
#define METACLASS_ROOT_BEGIN(NAME) \
    struct Class : public Metaclass \
    { \
    Class (const String& n, Metaclass* p = nullptr) : Metaclass (n, p) {}\
    static Class& instance() { const ScopedLock sl (Metaclass::sharedLock()); static Class meta(#NAME); return meta; }

/** Ends definition of the top-most Metaclass */
#define METACLASS_ROOT_END \
    }; \
    virtual Class* getClass() const { return &Class::instance(); }

/**
 Boilerplate that starts the definition of a Metaclass NAME::Class that derives from PARENT::Class
 Argument NAME is the class name currently in scope. Argument PARENT is its immediate superclass.
 User code can refer to 'Parent' as the superclass and 'Super' as the Metaclass superclass.
 */
#define METACLASS_BEGIN(NAME,PARENT) \
    using Parent = PARENT; \
    struct Class : public PARENT::Class \
    {   using Super = PARENT::Class; \
        Class (const String& n, Metaclass* p = nullptr) : Super (n,p) {}\
        static Class& instance() { const ScopedLock sl (Metaclass::sharedLock()); static Class meta(#NAME, &Super::instance()); return meta; }

/**
 Boilerplate that starts the definition of a Metaclass NAME::Class that derives from PARENT::Class
 plus another base class DERIVED. Argument NAME is the class name currently in scope. Argument PARENT
 is its immediate superclass. User code can refer to 'Parent' as the superclass and 'Super' as the
 Metaclass superclass.
 */
#define METACLASS_BEGIN_DERIVED(NAME,PARENT,DERIVED) \
    using Parent = PARENT; \
    struct Class : public PARENT::Class, public DERIVED \
    {   using Super = PARENT::Class; \
        Class (const String& n, Metaclass* p = nullptr) : Super (n,p) {}\
        static Class& instance() { const ScopedLock sl (Metaclass::sharedLock()); static Class meta(#NAME, &Super::instance()); return meta; }

/** Ends the definition of a derived Metaclass */
#define METACLASS_END \
    }; \
    Class* getClass() const override { return &Class::instance(); }

