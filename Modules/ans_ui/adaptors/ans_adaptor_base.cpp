/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_base.h"

#include "../core/ans_UIModel.h"
#include "../core/ans_UIInstance.h"
#include "../utility/ans_Positioners.h"

namespace ans {
    using namespace juce;
    
#if 0
#pragma mark UIAdaptor
#endif
    
    UIAdaptor::UIAdaptor (std::shared_ptr<UIInstance> instance, const ComponentSpec& spec) :
        identifier (spec.identifier),
        type (spec.type),
        ui (instance),
        insideUpdate (false),
        insideBuild (false)
    {
        for (auto t : Binding::Purpose::KnownTypes)
            bindings.add (nullptr);
        
        defaultAspect = spec.aspect;
    }
    
    UIAdaptor::~UIAdaptor ()
    {
        jassert (!insideBuild);
        
        if (ui != nullptr)
            ui->unregisterAdaptor (this);
    }
    
    std::shared_ptr<UIInstance> UIAdaptor::getUIInstance()
    {
        return ui;
    }
    
    UIModel* UIAdaptor::getModel()
    {
        if (ui.get() != nullptr)
            return ui->getModel();
        else
            return nullptr;
    }
    
    void UIAdaptor::initialiseFromSpec (std::shared_ptr<UIInstance> instance, const ComponentSpec& spec)
    {
        // The co-inherited Component must already been constructed!
        Component* comp = getComponent();
        
        jassert (comp != nullptr);
        jassert (getUIInstance() != nullptr);
        jassert (getModel() != nullptr);
        
        comp->setComponentID (spec.identifier);
        ui->registerAdaptor (this);
        
        for (auto c : spec.colours.assignments)
            comp->setColour (c.identifier, c.colour);
        
        // If we're building a mockup proxy, getModel() is a UIEditor!
        if (!instance->isMockup())
            for (auto b : spec.bindings)
                addBinding (b->instantiateFor (getModel()));
        
        spec.layout.applyToComponent (comp);
    }    
    
    void UIAdaptor::addComponent (std::unique_ptr<Component> comp)
    {
        getComponent()->addAndMakeVisible (comp.get());
        ownedComponents.add (std::move(comp));
    }
    
    void UIAdaptor::addComponent (std::unique_ptr<Component> comp, const LayoutFrame& frame)
    {
        comp->setPositioner (new FramePositioner (*comp, frame));
        addComponent (std::move(comp));
    }
    
    void UIAdaptor::addBinding (Binding* binding)
    {
        if (binding == nullptr)
            return;
        
        jassert (binding->isBound());
        
        if (bindings[binding->purpose] != nullptr)
        {
            DBG ("*** WARNING: Binding replaces existing binding");
        }
        
        bindings.set (binding->purpose, binding);
    }
    
    void  UIAdaptor::performBindingIfSet (Binding* binding)
    {
        if (binding != nullptr)
        {
#if DEBUG_UPDATES
            DBG ("      " << getComponent()->getComponentID().quoted() << " performs " << binding->purpose.name);
#endif
            binding->performFor(*this);
        }
    }
    
    void UIAdaptor::update (Model* sender, Aspect aspect, void* argument)
    {
        // Prevents infinite loop in response to whatever this update triggers
        if (insideUpdate)
            return;
        
        insideUpdate = true;
        
        for (auto binding : bindings)
            if (binding != nullptr && binding->isAspected())
            {
                if (aspect == Model::Undefined)
                {
                    // Undefined serves as a wildcard for selected purposes
                    if (binding->purpose.respondToUndefined)
                        performBindingIfSet (binding);
                }
                else if (aspect == binding->getAspect())
                {
                    performBindingIfSet (binding);
                }
            }
        
        insideUpdate = false;
    }
    
    void UIAdaptor::warn (const Binding::Purpose& p)
    {
        DBG ("Binding for component " << getComponent()->getComponentID().quoted() << " in " << getModel()->getClass()->getName() <<
             " does nothing on " << p.name << " - mismatched purpose or value type?");
        jassertfalse;
    }
    
}//
