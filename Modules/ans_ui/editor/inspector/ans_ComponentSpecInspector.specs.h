/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

/** This file is included early in the class defintion scope of ComponentSpecInspector */
    
DEFINE_SYMBOL (ComponentSelection);
DEFINE_SYMBOL (ComponentSettings);
DEFINE_SYMBOL (ComponentLayout);
    
METACLASS_BEGIN (ComponentSpecInspector, EmbeddedUIModel)

const String getNameSpace() override { return "ans"; }

String getSpecsFilename() const override { return __FILE__; }
    
/** Use these aspects for change propagation in ComponentSpecInspector */
Array<Aspect> getAspects() const override { return { ComponentSelection, ComponentSettings, ComponentLayout }; }

METACLASS_END
   
/** Use these globals to create pre-defined user interfaces for ComponentSpecInspector */
static WeakReference<UISpec> DefaultSpec;


