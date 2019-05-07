/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_adaptor_base.h"

namespace ans {
    using namespace juce;

/**
 UIComposite extends juce::Component to serve as a general 'canvas' for replaceable content.
 It supports adding children and features an optional background colour. The top-level content
 Component of a window is a UIComposite. Other than juce::Component, UIComposite owns all its
 children and deletes them when its contents are cleared or when UIComposite is deleted.
 */
    
class UIComposite :
        public juce::Component,
        public UIAdaptor
{
public:
    UIComposite (std::shared_ptr<UIInstance> instance, const CompositeSpecBase& spec);
    
    /** Constructor for ad-hoc use without a spec */
    UIComposite (std::shared_ptr<UIInstance> instance, const String& name);
   ~UIComposite ();
    
    UIComposite* getUIComposite() override { return this; };
    
    /** Empty the composite by removing and deleting any owned children */
    virtual void deleteContents();
    
    void setBackgroundColour (Colour colour);
    
    void resized() override;
    void paint (Graphics& g) override;
    
    void componentBuildBegin() override;
    void componentBuildEnd() override;

private:
    bool hasBackgroundColour;
    Colour backgroundColour;
};


}
