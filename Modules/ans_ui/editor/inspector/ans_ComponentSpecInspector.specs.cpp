/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_ComponentSpecInspector.h"

namespace ans {
using namespace juce;


INIT_SYMBOL (ComponentSpecInspector, ComponentSelection);
INIT_SYMBOL (ComponentSpecInspector, ComponentSettings);
INIT_SYMBOL (ComponentSpecInspector, ComponentLayout);

WeakReference<UISpec> ComponentSpecInspector::DefaultSpec = new UISpec
(
 ComponentSpecInspector::Class::instance(),
 "DefaultSpec",
 []()
 {
    auto content = new CompositeSpec ("content");
    {
        auto inspectorForm = new CanvasSpec ("inspectorForm");
        inspectorForm->addBinding (Bind::Canvas (MEMBER (&ComponentSpecInspector::populateCanvas), ComponentSelection));
        content->addComponent (inspectorForm);
    }
    return content;
},
true
);



} // namespace

