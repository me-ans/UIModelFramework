/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once
    
/** This file is included early in the class defintion scope of WidgetsExample */

DEFINE_SYMBOL (SpecSelection);
DEFINE_SYMBOL (LayoutSettings);
    
METACLASS_BEGIN (LayoutSpecInspector, EmbeddedUIModel)
    
String getSpecsFilename() const override { return __FILE__; }
    
Array<Aspect> getAspects() const override { return { SpecSelection, LayoutSettings }; }

const String getNameSpace() override { return "ans"; }

METACLASS_END

/** Refer to these globals to create user interfaces for WidgetsExample */
static WeakReference<UISpec> DefaultSpec;


