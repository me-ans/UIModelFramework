/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

/** This file is included early in the class defintion scope of UIEditor */
    
DEFINE_SYMBOL (TabContents);
DEFINE_SYMBOL (TabSelection);
DEFINE_SYMBOL (ModelSelection);
DEFINE_SYMBOL (ModelSettings);
DEFINE_SYMBOL (ModelAspects);
DEFINE_SYMBOL (SpecSelection);
DEFINE_SYMBOL (ComponentSelection);
DEFINE_SYMBOL (ComponentSettings);
DEFINE_SYMBOL (ComponentLayout);
DEFINE_SYMBOL (LayoutCanvas);

    
METACLASS_BEGIN (UIEditor, WindowUIModel)

const String getNameSpace() override { return "ans"; }

String getSpecsFilename() const override { return __FILE__; }
    
/** Use these aspects for change propagation in UIEditor */
Array<Aspect> getAspects() const override { return { TabContents, TabSelection, ModelSelection, ModelSettings, ModelAspects, SpecSelection, ComponentSelection, ComponentSettings, ComponentLayout, LayoutCanvas }; }

METACLASS_END
   
/** Use these globals to create pre-defined user interfaces for UIEditor */
static WeakReference<UISpec> MainWindowSpec;
static WeakReference<UISpec> TabSpecModel;
static WeakReference<UISpec> TabSpecLayout;
static WeakReference<UISpec> TabSpecGraphics;
static WeakReference<UISpec> TabSpecResources;
static WeakReference<UISpec> TabSpecCode;


