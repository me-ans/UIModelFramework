/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_UISpec.h"
#include "../core/ans_UIModel.h"

namespace ans {
    using namespace juce;
    
//==========================================================================================================
#if 0
#pragma mark UISpec
#endif

UISpec::UISpec (Model::Class* modelClassRef, const String& specName_, SpecLambda contentCreator, bool beDefault) :
    modelClass (modelClassRef),
    specName (specName_),
    specLambda (contentCreator),
    defaultSpec (beDefault)
{
    // @todo: This is odd!! Somehow need to get UIModel::Class as argument here with header re-ordering!
    auto mc = static_cast<UIModel::Class*>(modelClass);
    mc->addSpec (this);
}

UISpec::UISpec (Model::Class* modelClassRef, std::unique_ptr<ComponentSpec> content) :
    modelClass (modelClassRef),
    specName ("Temporary"),
    specLambda ([](){ return nullptr; }),
    defaultSpec (false)
{
    // Temp specs are NOT registered globally
    rootSpec = std::move(content);
}

UISpec::~UISpec ()
{
    masterReference.clear();
}

void UISpec::flush ()
{
    rootSpec = nullptr;
}

ComponentSpec* UISpec::getRootComponentSpec ()
{
    if (rootSpec == nullptr)
        rootSpec = createSpec();

    return rootSpec.get();
}

std::unique_ptr<ComponentSpec> UISpec::createSpec ()
{
    if (specLambda != nullptr)
    {
        DBG ("Creating " << getModelClass()->getName() << "::" << getName());
        // @todo: specLambda still uses new() and raw pointers
        return std::unique_ptr<ComponentSpec> (specLambda());
    }
    return nullptr;
}

Aspects UISpec::getUsedAspects ()
{
    if (auto spec = getRootComponentSpec())
        return spec->getUsedAspects();
    else
        return Aspects();

}

ComponentSpec*  UISpec::getComponentSpec (const ComponentID& identifier)
{
    return findComponentSpec (identifier, getRootComponentSpec());
}

const ComponentID  UISpec::getUniqueComponentID (const ComponentID& identifier, ComponentSpec* exclude)
{
    ComponentID id = identifier;
    int iter = 0;
    
    while (auto found = findComponentSpec (id, getRootComponentSpec(), exclude))
    {
        id = id.removeCharacters ("0123456789") + String(++iter);
    }
    return id;
}

ComponentSpec* UISpec::findComponentSpec (const ComponentID& identifier, ComponentSpec* current, ComponentSpec* exclude)
{
    if (current == nullptr)
        return nullptr;
    
    if (current != exclude && current->identifier == identifier)
        return current;
    
    for (auto child : current->children)
        if (auto found = findComponentSpec (identifier, child))
            return found;
    
    return nullptr;
}


String UISpec::generateSourceCPP (Model::Class* modelClass)
{
    /***************************************************************************************
     CAUTION! This includes some awkward 'here docs' - don't get confused by syntax coloring
     ***************************************************************************************/
    
    static const String form = R"HERE(
WeakReference<UISpec> $MODEL::$NAME = new UISpec
(
 $MODEL::getMetaClass(),
 "$NAME",
 []()
 {$SPEC}$DEFAULT
);

)HERE";
    
    String out;
    out << form.replace ("$MODEL", modelClass->getName())
               .replace ("$NAME", getName())
               .replace ("$DEFAULT", (isDefault() ? ",\ntrue" : "" ))
               .replace ("$SPEC", getRootComponentSpec()->generateSourceCPP (modelClass));
    out << "\n\n";
    return out;
}


//==========================================================================================================
#if 0
#pragma mark UISpecRegistry
#endif

UISpecRegistry::UISpecRegistry ()
{
}

UISpecRegistry::~UISpecRegistry ()
{
    shuttingDown = true;
    
    for (UISpec* each : getUISpecs())
        if (each != nullptr)
            delete each;
    
    specs.clear();
    masterReference.clear();
}

void UISpecRegistry::addSpec (UISpec* spec)
{
    if (shuttingDown)
        return;
    
    for (auto& each : specs)
    {
        // Already there?
        if (each.get() == spec)
            return;
        // Replace if same name
        if (each.get() != nullptr && each.get()->getName() == spec->getName())
        {
            each = spec;
            return;
        }
    }
    // Add new
    specs.add (WeakReference<UISpec> (spec));
}

Array<UISpec*> UISpecRegistry::getUISpecs()
{
    Array<UISpec*> result;
    for (auto& each : specs)
    {
        if (each.get() != nullptr)
            result.add (each.get());
    }
    collectGarbage();
    return result;
}

void UISpecRegistry::collectGarbage()
{
    specs.removeIf ([](auto& each) { return each.get() == nullptr; });
}
    
}
