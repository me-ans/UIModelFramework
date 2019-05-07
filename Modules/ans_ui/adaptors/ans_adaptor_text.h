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
    
/** UITextEditor extends TextEditor for communication with UIModel */

class UITextEditor :
        public juce::TextEditor,
        public UIAdaptor
{
public:
    UITextEditor (std::shared_ptr<UIInstance> instance, const TextSpecBase& spec);
   ~UITextEditor();
    
    void getComponentState (const Binding::Purpose& p, var& value) override;
    void setComponentState (const Binding::Purpose& p, const var& value) override;
    
    void focusGained (FocusChangeType cause) override;
    
private:
    void textEditorTextChanged ();
    void textEditorReturnKeyPressed ();
    void textEditorEscapeKeyPressed ();
    void textEditorFocusLost ();

    bool acceptKeyStrokes;
    bool hasChanged;
    String backup;
};

/** UICodeEditor extends CodeEditorComponent for communication with UIModel */

class UICodeEditor :
    public juce::Component,
    public UIAdaptor
{
public:
    UICodeEditor (std::shared_ptr<UIInstance> instance, const TextSpecBase& spec);
   ~UICodeEditor();
    
    void getComponentState (const Binding::Purpose& p, var& value) override;
    void setComponentState (const Binding::Purpose& p, const var& value) override;
    
private:
    CodeDocument document;
    CPlusPlusCodeTokeniser tokeniser;
    std::unique_ptr<CodeEditorComponent> editor;
    LookAndFeel_V4 lookAndFeel;
};


}
