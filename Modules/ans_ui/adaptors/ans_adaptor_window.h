/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_adaptor_base.h"
#include "../core/ans_UIInstance.h"

namespace ans {
    using namespace juce;

/**
 UIWindowBase is base class for adaptors that coopretae with WindowUIModel.
 */
class UIWindowBase : public UIAdaptor
{
public:
    /** The window takes ownership of the UIInstance! Therefore it is important to pass a new instance here. */
    UIWindowBase (UIInstance* instance, const WindowSpec& spec);
   ~UIWindowBase ();
    
    bool isWindowAdaptor() override { return true; }
    
    void update (Model* sender, Aspect aspect, void* argument) override;
    
    /** Deletes and closes this window and notifies its WindowUIModel  */
    void close();

     /** Re-deploy changes made to the UISpec regarding layout to all registered components (live editing) */
    void updateLayoutFromSpec();
    
protected:
    UIInstance::Ptr uiInstance;
    ScopedPointer<LookAndFeel> look;
    
};


/**
 UIDocumentWindow extends DocumentWindow in order to integrate with UIBuilder and WindowUIModel.
 */
class UIDocumentWindow :
        public juce::DocumentWindow,
        public UIWindowBase
{
public:
    /**
     Constructs a DocumentWindow according to a WindowSpec. The window takes ownership of the UIInstance!
     Therefore it is important to pass a new instance here.
     */
    UIDocumentWindow (UIInstance* instance, const WindowSpec& spec);
   ~UIDocumentWindow ();
    
    UIComposite* getUIComposite() override;
    
    void setComponentState (const Binding::Purpose& p, const String& value) override { DocumentWindow::setName (value); }
    
    void resized() override;
    void closeButtonPressed () override;
};

}
