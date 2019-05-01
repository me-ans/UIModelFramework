/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_LayoutSpecInspector.h"

namespace ans {
    using namespace juce;

INIT_SYMBOL (LayoutSpecInspector, SpecSelection);
INIT_SYMBOL (LayoutSpecInspector, LayoutSettings);

WeakReference<UISpec> LayoutSpecInspector::DefaultSpec = new UISpec
(
 LayoutSpecInspector::Class::instance(),
"DefaultSpec",
[]()
{
    auto content = new CompositeSpec ("content");
    {
        auto form = new CanvasSpec ("form");
        form->setBackgroundColour (SOURCE (Colours::darkkhaki));
        form->addBinding (Bind::Canvas (MEMBER (&LayoutSpecInspector::buildCanvas), SpecSelection));
        content->addComponent (form);
    }
    return content;
},
 true);


}//
