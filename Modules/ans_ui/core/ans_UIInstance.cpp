/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

//#include "ans_UIInstance.h"
//#include "ans_UIModel.h"

namespace ans
{
    using namespace juce;

//==========================================================================================================
#if 0
#pragma mark UIInstance
#endif

UIInstance::UIInstance (UIModel* uiModel, UISpec* uiSpec, std::shared_ptr<UIInstance> parentInstance, bool forMockup) :
    parent (parentInstance),
    spec (uiSpec),
    model (uiModel),
    registry (),
    mockup (forMockup)
{
}

UIInstance::~UIInstance ()
{
    clear();
    
    if (auto instance = parent.lock())
        instance->childWasDeleted (this);
}

void UIInstance::clear()
{
    Registry::Iterator cursor (registry);
    while (cursor.next())
        if (cursor.getValue() != nullptr && !cursor.getValue()->isWindowAdaptor())
        {
            DBG ("*** WARNING: Cleaning up leaked adaptor " << cursor.getValue()->identifier.quoted());
            delete cursor.getValue();
        }
    
    registry.clear();
}

std::shared_ptr<UIInstance> UIInstance::getChildInstanceFor (UIModel* uiModel, UISpec* uiSpec)
{
    if (auto instance = findChildFor (uiModel, uiSpec))
        return instance;
    
    auto instance = std::make_shared<UIInstance> (uiModel, uiSpec, shared_from_this());
    children.add (instance);
    return instance;
}

bool UIInstance::isForWindow()
{
    return model->isWindowModel();
}

bool UIInstance::registerAdaptor (UIAdaptor* adaptor)
{
    if (adaptor == nullptr)
        return false;
    
    if (auto existing = registry[adaptor->identifier])
    {
        if (existing == adaptor)
            return true;
        
        /** @todo: It's probably a better idea to keep this identifier only in one place? */
        jassert (adaptor->identifier == adaptor->getComponent()->getComponentID());
        DBG ("*** WARNING: " << model->getClass()->getName() << " replacing adaptor with identical id " << adaptor->identifier.quoted());
        unregisterAdaptor (existing);
    }
    
    //DBG (model->getClass().getName() << " registering adaptor " << adaptor->identifier.quoted());
    registry.set (adaptor->identifier, adaptor);
    model->addDependent (adaptor);
    return true;
}

void UIInstance::unregisterAdaptor (UIAdaptor* adaptor)
{
    if (adaptor == nullptr)
        return;
    
    if (model.get())
    {
        //DBG (model->getClass().getName() << " unregistering adaptor " << adaptor->identifier.quoted());
        model->removeDependent (adaptor);
    }
    registry.remove (adaptor->identifier);
}

UIAdaptor* UIInstance::getAdaptor (const ComponentID& identifier, bool lookIntoChildren)
{
    if (auto adaptor = registry[identifier])
        return adaptor;
    
    if (!lookIntoChildren)
        return nullptr;
    
    for (auto child : children)
        if (auto adaptor = child->getAdaptor (identifier, lookIntoChildren))
            return adaptor;
    
    return nullptr;
}

Component* UIInstance::getComponent (const ComponentID& identifier, bool lookIntoChildren)
{
    if (auto adaptor = getAdaptor (identifier, lookIntoChildren))
        return adaptor->getComponent();
    else
        return nullptr;
}

std::shared_ptr<UIInstance> UIInstance::getMockupUI()
{
    if (mockupUI == nullptr)
        mockupUI = std::make_shared<UIInstance> (getModel(), getSpec(), parent.lock(), true);
    
    return mockupUI;
}

String UIInstance::printDebug()
{
    return String("UIInstance{") + model->getClass()->getName() + "." + spec->getName() + "}";
}

void UIInstance::childWasDeleted (UIInstance* child)
{
    children.removeIf ([&](auto each){ return each.get() == child; });
}

std::shared_ptr<UIInstance> UIInstance::findChildFor (UIModel* m, UISpec* s)
{
    for (auto existing : children)
        if (existing->getModel() == m && existing->getSpec() == s)
            return existing;

    return nullptr;
}

void UIInstance::updateLayoutFromSpec()
{
    if (auto rootSpec = getSpec()->getRootComponentSpec())
    {
        updateLayoutFromSpec (rootSpec);
        if (auto adaptor = getAdaptor (rootSpec->identifier))
            if (auto comp = adaptor->getComponent())
                comp->resized();
    }
}

void UIInstance::updateLayoutFromSpec (ComponentSpec* spec)
{
    if (spec == nullptr)
        return;
    
    if (auto adaptor = getAdaptor (spec->identifier))
        spec->layout.applyToComponent (adaptor->getComponent());
    
    for (auto child : spec->children)
        updateLayoutFromSpec (child);
}


} //
