/**
 Class Extensions For Juce
 -------------------------
 Copyright 2019 me-ans@GitHub
 Please regard the license terms accompanying this Juce module.
 */

#include "ans_Model.h"

namespace ans
{
    using namespace juce;

//================================================================================================
#if 0
#pragma mark Aspects
#endif

Aspects::Aspects () {}

Aspects::Aspects (Aspect a) { add (a); }

Aspects::Aspects (std::initializer_list<Aspect> list)
{
    for (auto a : list)
        add (a);
}

String Aspects::toString () const
{
    String out;
    for (auto a : contents)
    {
        if (out.isNotEmpty()) out << ", ";
        out << Symbol(a).toString();
    }
    return out;
}

//================================================================================================
#if 0
#pragma mark Model
#endif

INIT_SYMBOL (Model, Undefined);


Model::Model() noexcept
{
}

Model::~Model()
{
}

void Model::addDependent (Dependent* const dependent)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    dependents.add (dependent);
}

void Model::removeDependent (Dependent* const dependent)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    dependents.remove (dependent);
}

void Model::removeAllDependents()
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    dependents.clear();
}

void Model::changed (const Array<Aspect>& aspects)
{
    for (auto a : aspects)
        changed(a);
}

void Model::changed (Aspect aspect, void* parameter)
{
#if ANS_DEBUG_UPDATES
    DBG (CLASSNAME << " changed (" << printAspect(aspect) << ")");
#endif
    if (dependents.size() > 0)
        callDependents (aspect, parameter);
}

void Model::changedAspects (const Aspects& aspects)
{
#if ANS_DEBUG_UPDATES
    DBG (CLASSNAME << " changed (" << aspects.toString() << ")");
#endif
    for (auto a : aspects.contents)
        changed(a);
}

void Model::callDependents (Aspect aspect, void* argument)
{
    dependents.call ([this, aspect, argument]
                     (Dependent& dep)
                     { dep.update (this, aspect, argument); });
}


//================================================================================================
#if 0
#pragma mark Model::Class
#endif

Array<Aspect> Model::Class::getAllAspects() const
{
    if (auto parent = dynamic_cast<Model::Class*>(getSuperClass()))
    {
        Array<Aspect> result = parent->getAllAspects();
        result.addArray (getAspects());
        return result;
    } else
        return getAspects();
}

Array<String> Model::Class::getAspectNames() const
{
    Array<String> answer;
    for (auto a : getAspects())
        answer.add (Symbol(a).toString());
    
    return answer;
}

String Model::Class::printAspect (Aspect a) const
{
    return Symbol(a).toString();
}

Aspect Model::Class::parseAspect (const String& name, bool warn) const
{
    Symbol symbol (name);
    auto known = getAllAspects();
    if (known.indexOf (symbol) < 0)
    {
        if (warn)
        {
            DBG ("*** ERROR: Aspect " << name.quoted() << " unknown in scope of " << getName());
        }
        return Model::Undefined;
    }
    return symbol;
}

}
