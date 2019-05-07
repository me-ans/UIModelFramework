/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_composite.h"

#include "../specs/ans_UIComponentClass.h"
#include "../utility/ans_Positioners.h"

namespace ans {
    using namespace juce;

UIComposite::UIComposite (std::shared_ptr<UIInstance> instance, const CompositeSpecBase& spec) :
    Component (spec.identifier),
    UIAdaptor (instance, spec),
    hasBackgroundColour (false)
{
    initialiseFromSpec (instance, spec);
    
    if (spec.hasBackgroundColour())
        setBackgroundColour (spec.backgroundColour.colour);
}

UIComposite::UIComposite (std::shared_ptr<UIInstance> instance, const String& name) :
    Component (name),
    UIAdaptor (instance, ComponentSpec (UIComponentClass::Type::Composite, name)), // dummy spec
    hasBackgroundColour (false)
{
}

UIComposite::~UIComposite ()
{
}

void UIComposite::deleteContents()
{
    ownedComponents.clear();
}

void UIComposite::resized()
{
    if (auto parent = getParentComponent())
    {
        if (auto positioner = getPositioner())
            positioner->applyNewBounds (parent->getBounds());
    }
}

void UIComposite::paint (Graphics& g)
{
   if (hasBackgroundColour)
       g.fillAll (backgroundColour);
    Component::paint (g);
}

void UIComposite::setBackgroundColour (Colour colour)
{
    backgroundColour = colour;
    hasBackgroundColour = true;
    colourChanged();
}

void UIComposite::componentBuildBegin ()
{
    insideBuild = true;
    setVisible (false);
    deleteContents();
}

void UIComposite::componentBuildEnd ()
{
    resized();
    sendLookAndFeelChange();
    setVisible (true);
    toFront (true);
    insideBuild = false;
}


}//
