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
 */
class UIComposite :
        public juce::Component,
        public UIAdaptor
{
public:
    UIComposite (UIInstance* owner, const CompositeSpecBase& spec);
    
    /** Constructor for ad-hoc use without a spec */
    UIComposite (UIInstance* owner, const String& name);
   ~UIComposite ();
    
    UIComposite* getUIComposite() override { return this; };
    
    /** Empty the composite by removing and deleting any owned children */
    virtual void deleteContents();
    
    /**
     Add a child component bounded by a given LayoutFrame. 
     UIComposite owns its children and takes care of deleting them!
     */
    virtual void addComponent (Component* comp, const LayoutFrame& frame);
    
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
