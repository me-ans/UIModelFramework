/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_widgets.h"

namespace ans {
    using namespace juce;

//==========================================================================================================
#if 0
#pragma mark UIRadioButton
#endif

UIRadioButton::UIRadioButton (UIInstance* owner, const RadioSpec& spec) :
    ToggleButton (spec.identifier),
    UIAdaptor (owner, spec),
    value (spec.groupValue),
    groupMaster (-1)
{
    initialiseFromSpec (owner, spec);
    
    if (!spec.label.isEmpty())
        setButtonText (spec.label);
    
    setClickingTogglesState (false);
    setRadioGroupId (spec.groupId);
    setConnectedEdges (spec.groupConnections);
}
    
bool  UIRadioButton::isMaster ()
{
    ensureMasterRoleSet();
    return groupMaster == 1;
}

void  UIRadioButton::setMaster (bool master)
{
    groupMaster = master ? 1 : 0;
}

void UIRadioButton::ensureMasterRoleSet()
{
    if (groupMaster >= 0)
        return;

    bool haveMaster = false;
    enumerateButtonGroup ([] (auto b){ return true; },
                          [&](auto b)
                          {
                              if (!haveMaster && (b->hasBinding (Binding::Purpose::SetSelection) || b->hasBinding (Binding::Purpose::GetSelection)))
                              {
                                  b->setMaster (true);
                                  haveMaster = true;
                              } else
                                  b->setMaster (false);
                          });
    // Your button group needs at least one with a GetSelection or SetSelection binding!
    jassert (haveMaster);
}

var UIRadioButton::getGroupSelection()
{
    ensureMasterRoleSet();
    var result;
    enumerateButtonGroup ([] (auto b){ return b->getToggleState(); },
                          [&](auto b){ result = b->value; });
    return result;
}

void UIRadioButton::setGroupSelection (const var& newValue)
{
    ensureMasterRoleSet();
    enumerateButtonGroup ([&](auto b){ return b->value.equals (newValue); },
                          [] (auto b){ b->setToggleState (true, dontSendNotification); });
}

void UIRadioButton::clicked ()
{
    setGroupSelection (getButtonValue());
    
    // Have only the master button carry out its bindings
    enumerateButtonGroup ([&](auto b){ return b->isMaster(); },
                          [&](auto b){
                              b->performBinding (Binding::Purpose::SetSelection);
                              b->performBinding (Binding::Purpose::Action);
                          });
}


//==========================================================================================================
#if 0
#pragma mark UISlider
#endif

UISlider::UISlider (UIInstance* owner, const SliderSpecBase& spec) :
    Slider (spec.identifier),
    UIAdaptor (owner, spec)
{
    initialiseFromSpec (owner, spec);
    
    setRange (spec.valueRange.getStart(), spec.valueRange.getEnd(), spec.valueInterval);
    
    if (spec.type == UIComponentClass::Type::Entry)
    {
        setIncDecButtonsMode (Slider::incDecButtonsDraggable_Horizontal);
    }
    else if (spec.type == UIComponentClass::Type::Knob)
    {
        setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    }
    else
    {
        auto box = spec.layout.frame.rectangleRelativeTo ({0, 0, 200, 200});
        if (box.getWidth() > box.getHeight())
            setSliderStyle (Slider::LinearHorizontal);
        else
            setSliderStyle (Slider::LinearVertical);
    }
    
    addListener (this);
}

UISlider::~UISlider ()
{
    removeListener (this);
}

//==========================================================================================================
#if 0
#pragma mark UIComboBox
#endif

UIComboBox::UIComboBox (UIInstance* owner, const ComboSpecBase& spec) :
    ComboBox (spec.identifier),
    UIAdaptor (owner, spec)
{
    initialiseFromSpec (owner, spec);
    if (spec.type == UIComponentClass::Type::Popup)
    {
        setEditableText (false);
    }
    if (spec.type == UIComponentClass::Type::Combo)
    {
        setEditableText (true);
    }
    onChange = [this](){ selectionChanged(); };
}

UIComboBox::~UIComboBox ()
{
    onChange = [](){};
}

void UIComboBox::getComponentState (const Binding::Purpose& p, String& value)
{
    if (p == Binding::Purpose::SetValue)
    {
        value = getText();
        return;
    }
    if (p == Binding::Purpose::SetSelection)
    {
        value = getItemText (getSelectedItemIndex());
        return;
    }
}

void UIComboBox::getComponentState (const Binding::Purpose& p, int& value)
{
    if (p == Binding::Purpose::SetSelection)
    {
        value = getSelectedId();
        return;
    }
}

void UIComboBox::setComponentState (const Binding::Purpose& p, const String& value)
{
    if (p == Binding::Purpose::GetValue || p == Binding::Purpose::GetLabel)
        return setText (value, dontSendNotification);
}

void UIComboBox::setComponentState (const Binding::Purpose& p, int value)
{
    if (p == Binding::Purpose::GetSelection)
        return setSelectedId (value, dontSendNotification);
}

void UIComboBox::setComponentState (const Binding::Purpose& p, UIValueMap& value)
{
    if (p == Binding::Purpose::GetValue)
    {
        // @todo: This should be a MenuSpec or something rather than this raw data
        ComboBox::clear (dontSendNotification);
        for (auto pair : value)
            if (pair.first == -1)
                addSectionHeading (pair.second);
            else
                addItem (pair.second, pair.first);
        return;
    }
}

void UIComboBox::selectionChanged()
{
    performBinding (Binding::Purpose::SetSelection);
}

}

