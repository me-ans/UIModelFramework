/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "JuceHeader.h"
#include "ans_LayoutFrame.h"

namespace ans {
    using namespace juce;

/**
 FramePositioner positions a Component according to a LayoutFrame that dynamically
 determines the component's bounds relative to it's parent's bounds.
 FramePositioner & LayoutFrame are intended as a simple and fast alternative for
 components that don't need full FlexBox capabilities. It especially lends itself
 to layouts that get built incrementally (programmatically) with LayoutCursor.
 
 @see LayoutFrame, LayoutCursor
 */
struct FramePositioner : Component::Positioner
{
    FramePositioner (Component &target,  const LayoutFrame& f) :
        juce::Component::Positioner (target),
        frame (f)
    {
    }
    
    void applyNewBounds (const juce::Rectangle<int>& parentRect) override
    {
        Rectangle<int> bounds = frame.rectangleRelativeTo (parentRect);
        getComponent().setBounds (bounds);
        
        for (auto child : getComponent().getChildren())
            if (auto positioner = child->getPositioner())
                positioner->applyNewBounds (bounds);
    }
    
    const LayoutFrame frame;
};
    
/**
 GridPositioner positions a Component according to a Grid, GridItem which dynamically
 determine the component's bounds relative to it's parent's bounds. The Grid setting
 applies, if the component is a container for children that each posess a GridItem setting.
 */
struct GridPositioner : Component::Positioner
{
    GridPositioner (Component &target, const Grid &g, const GridItem &i) :
        Component::Positioner(target),
        grid(g),
        gridItem(i)
    {
        gridItem.associatedComponent = &target;
    }
    
    void applyNewBounds (const Rectangle<int> &area) override
    {
        grid.items.clearQuick();
        for (Component *child : getComponent().getChildren())
        {
            auto *positioner = dynamic_cast<GridPositioner*>(child->getPositioner());
            if (positioner)
                grid.items.add (positioner->gridItem);
        }
        
        grid.performLayout (getComponent().getBounds());
        
        for (auto child : getComponent().getChildren())
            if (auto positioner = child->getPositioner())
                positioner->applyNewBounds({});
    }
    
    Grid     grid;
    GridItem gridItem;
};

/**
 FlexPositioner positions a Component according to a FlexBox, FlexItem which dynamically
 determine the component's bounds relative to it's parent's bounds. The FlexBox setting
 applies, if the component is a container for children that each posess a FlexItem setting.
 */
struct FlexPositioner : Component::Positioner
{
    FlexPositioner (Component &target, const FlexBox &b, const FlexItem &i) :
        Component::Positioner(target),
        flexBox(b),
        flexItem(i)
    {
        flexItem.associatedComponent = &target;
    }
    
    void applyNewBounds (const Rectangle<int> &area) override
    {
        flexBox.items.clearQuick();
        for (Component *child : getComponent().getChildren())
        {
            auto *positioner = dynamic_cast<FlexPositioner*>(child->getPositioner());
            if (positioner)
                flexBox.items.add (positioner->flexItem);
        }
        
        flexBox.performLayout (getComponent().getBounds());
        
        for (auto child : getComponent().getChildren())
            if (auto positioner = child->getPositioner())
                positioner->applyNewBounds({});
    }
    
    FlexBox  flexBox;
    FlexItem flexItem;
};



}
