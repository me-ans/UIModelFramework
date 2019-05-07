/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_text.h"

#include "../utility/ans_Positioners.h"

namespace ans {
    using namespace juce;

//==========================================================================================================
#if 0
#pragma mark UITextEditor
#endif

UITextEditor::UITextEditor (std::shared_ptr<UIInstance> instance, const TextSpecBase& spec) :
    TextEditor (spec.identifier),
    UIAdaptor (instance, spec),
    acceptKeyStrokes (false),
    hasChanged (false)
{
    initialiseFromSpec (instance, spec);
    
    if (spec.type == UIComponentClass::Type::Text)
    {
        setMultiLine (true);
        setScrollbarsShown (spec.enableScrollbars);
    }
    
    if (spec.type == UIComponentClass::Type::Input)
    {
        setMultiLine (false);
        setScrollbarsShown (false);
    }
    
    onTextChange = [this]() { textEditorTextChanged(); };
    onReturnKey  = [this]() { textEditorReturnKeyPressed(); };
    onEscapeKey  = [this]() { textEditorEscapeKeyPressed(); };
    onFocusLost  = [this]() { textEditorFocusLost(); };
}

UITextEditor::~UITextEditor ()
{
    onTextChange = []() { };
    onReturnKey  = []() { };
    onEscapeKey  = []() { };
    onFocusLost  = []() { };
}

void UITextEditor::getComponentState (const Binding::Purpose& p, var& value)
{
    // Note: Uses var, because text input also works for int, double, etc
    if (p == Binding::Purpose::SetValue)
    {
        value = getText();
        return;
    }
    UIAdaptor::getComponentState(p, value);
}

void UITextEditor::setComponentState (const Binding::Purpose& p, const var& value)
{
    // Note: Uses var, because text input also works for int, double, etc
    if (p == Binding::Purpose::GetValue)
        return setText (value, dontSendNotification);
    
    UIAdaptor::setComponentState(p, value);
}

void UITextEditor::textEditorTextChanged ()
{
    hasChanged = true;
    if (acceptKeyStrokes)
        performBinding (Binding::Purpose::SetValue);
}

void UITextEditor::textEditorReturnKeyPressed ()
{
    if (!acceptKeyStrokes && type == UIComponentClass::Type::Input)
        performBinding (Binding::Purpose::SetValue);
}

void UITextEditor::textEditorEscapeKeyPressed ()
{
    setText (backup);
    hasChanged = false;
    performBinding (Binding::Purpose::SetValue);
    repaint();
}

void UITextEditor::textEditorFocusLost ()
{
    if (!acceptKeyStrokes && hasChanged && getText() != backup)
    {
        hasChanged = false;
        performBinding (Binding::Purpose::SetValue);
    }
}

void UITextEditor::focusGained (FocusChangeType cause)
{
    TextEditor::focusGained (cause);
    backup = TextEditor::getText();
    hasChanged = false;
}


//==========================================================================================================
#if 0
#pragma mark UICodeEditor
#endif

UICodeEditor::UICodeEditor (std::shared_ptr<UIInstance> instance, const TextSpecBase& spec) :
    Component (spec.identifier),
    UIAdaptor (instance, spec),
    document (),
    tokeniser ()
{
    initialiseFromSpec (instance, spec);
    editor = std::make_unique<CodeEditorComponent> (document, &tokeniser);
    editor->setPositioner (new FramePositioner (*editor, LayoutFrame::entire()));
    addAndMakeVisible (editor.get());
    
    lookAndFeel.setColourScheme (LookAndFeel_V4::getLightColourScheme());
    editor->setLookAndFeel(&lookAndFeel);
    editor->sendLookAndFeelChange();
}

UICodeEditor::~UICodeEditor ()
{
    if (editor != nullptr)
        editor->setLookAndFeel(nullptr);
}

void UICodeEditor::getComponentState (const Binding::Purpose& p, var& value)
{
    if (p == Binding::Purpose::SetValue)
    {
        value = document.getAllContent();
        return;
    }
    UIAdaptor::getComponentState(p, value);
}

void UICodeEditor::setComponentState (const Binding::Purpose& p, const var& value)
{
    if (p == Binding::Purpose::GetValue)
        return document.replaceAllContent (value);
    
    UIAdaptor::setComponentState(p, value);
}

}
