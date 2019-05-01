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

UIComposite::UIComposite (UIInstance* owner, const CompositeSpecBase& spec) :
    Component (spec.identifier),
    UIAdaptor (owner, spec),
    hasBackgroundColour (false)
{
    initialiseFromSpec (owner, spec);
    
    if (spec.hasBackgroundColour())
        setBackgroundColour (spec.backgroundColour.colour);
}

UIComposite::UIComposite (UIInstance* owner, const String& name) :
    Component (name),
    UIAdaptor (owner, ComponentSpec (UIComponentClass::Type::Composite, name)), // dummy spec
    hasBackgroundColour (false)
{
}

UIComposite::~UIComposite ()
{
    deleteAllChildren();
}

void UIComposite::deleteContents()
{
    deleteAllChildren();
}

void UIComposite::addComponent (Component* comp, const LayoutFrame& frame)
{
    comp->setPositioner (new FramePositioner (*comp, frame));
    addAndMakeVisible (comp);
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
