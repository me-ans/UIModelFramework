
/**
 This file is automatically generated by UIEditor. It defines the layout and data bindings of
 all UISpec owned by the associated UIModel. Include the UIEditor module with your development
 build in order to browse, open and edit all UISpec in your application at runtime.
 
 (Last generated 28 Apr 2019 8:48:31pm)
 */

#pragma once
    
/** This file is included early in the class defintion scope of CustomExample */
    
DEFINE_SYMBOL (Settings);
DEFINE_SYMBOL (Colours);
    
METACLASS_BEGIN (CustomExample, EmbeddedUIModel)
    
String getSpecsFilename() const override { return __FILE__; }
    
/** Use these aspects for change propagation in CustomExample */
Array<Aspect> getAspects() const override { return { Settings, Colours }; }

METACLASS_END
   
/** Use these globals to create pre-defined user interfaces for CustomExample */
static WeakReference<UISpec> DefaultSpec;

