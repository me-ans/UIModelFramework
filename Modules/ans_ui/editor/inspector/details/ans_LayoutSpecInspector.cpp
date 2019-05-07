/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_LayoutSpecInspector.h"
#include "../../ans_UIEditor.h"

namespace ans {
    using namespace juce;

LayoutSpecInspector::LayoutSpecInspector (ComponentSpecInspector* parent) :
    EmbeddedUIModel (parent),
    mainInspector (parent)
{
}

LayoutSpec& LayoutSpecInspector::getLayoutSpec()
{
    if (auto spec = mainInspector->getSelectedComponentSpec())
        return spec->layout;
    
    return dummy;
}

void LayoutSpecInspector::populateInspector (CompositeSpec* composite, LayoutCursor& cursor)
{
    if (mainInspector->getSelectedComponentSpec() == nullptr)
        return;
    
    cursor.setCellSize ({0.33, 22});
    cursor.setSpacing ({0,1});
    
    mainInspector->makeHeader(composite,
                              cursor,
                              "title",
                              "Layout");
    
    mainInspector->makeInput (composite,
                              cursor,
                              "inputLeft",
                              "Left",
                              MEMBER (&LayoutSpecInspector::getLeft),
                              MEMBER (&LayoutSpecInspector::setLeft),
                              LayoutSettings);
    
    mainInspector->makeInput (composite,
                              cursor,
                              "inputTop",
                              "Top",
                              MEMBER (&LayoutSpecInspector::getTop),
                              MEMBER (&LayoutSpecInspector::setTop),
                              LayoutSettings);
    
    mainInspector->makeInput (composite,
                              cursor,
                              "inputRight",
                              "Right",
                              MEMBER (&LayoutSpecInspector::getRight),
                              MEMBER (&LayoutSpecInspector::setRight),
                              LayoutSettings);
    
    mainInspector->makeInput (composite,
                              cursor,
                              "inputBottom",
                              "Bottom",
                              MEMBER (&LayoutSpecInspector::getBottom),
                              MEMBER (&LayoutSpecInspector::setBottom),
                              LayoutSettings);    
}

void LayoutSpecInspector::update()
{
    changed (LayoutSettings);
    mainInspector->updateLayout();
}

}
