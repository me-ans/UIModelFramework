/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_ComponentSpecInspector.h"
#include "../ans_UIEditor.h"

namespace ans {
    using namespace juce;
    
    
UIEditor* ComponentSpecInspector::getUIEditor ()
{
    return dynamic_cast<UIEditor*> (getParentModel());
}

UISpec* ComponentSpecInspector::getSelectedUISpec ()
{
    return getUIEditor()->getSelectedUISpec();
}

const ComponentID ComponentSpecInspector::getSuggestedID (const ComponentID& base)
{
    return getUIEditor()->getSuggestedID (base);
}

void ComponentSpecInspector::updateLayout()
{
    changed (ComponentLayout);
    getUIEditor()->updateLayoutCanvas();
}


}
