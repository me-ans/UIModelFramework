/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_adaptor_base.h"
#include "../core/ans_UIModel.h"
#include "../utility/ans_Positioners.h"

namespace ans {
    using namespace juce;

/**
 UIProgressBar extends ProgressBar in that it maintains its own double variable
 updated via data binding to a UIModel.
 */
class UIProgressBar :
        public juce::ProgressBar,
        public UIAdaptor
{
public:
    UIProgressBar (std::shared_ptr<UIInstance> instance, const ProgressSpec& spec) :
        ProgressBar (progress),
        UIAdaptor (instance, spec),
        progress (0.0)
    {
        initialiseFromSpec (instance, spec);
        
        if (!spec.label.isEmpty())
            setTextToDisplay (spec.label);       
    }

    void setComponentState (const Binding::Purpose& p, const String& value) override { setTextToDisplay (value); }
    void getComponentState (const Binding::Purpose& p, double& value) override { value = progress; }
    void setComponentState (const Binding::Purpose& p, double  value) override { progress = value; repaint(); }
    
private:
    double progress;
};


/** UIButton extends various subclasses of juce::Button with a communication channel to UIModel */

template <typename ButtonClass>
class UIButton :
        public ButtonClass,
        public UIAdaptor
{
public:
    UIButton (std::shared_ptr<UIInstance> instance, const ButtonSpecBase& spec) :
        ButtonClass (spec.identifier),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
        if (!spec.label.isEmpty())
            ButtonClass::setButtonText (spec.label);
    }
    
    ~UIButton () {}
    
    void setComponentState (const Binding::Purpose& p, const String& value) override
    {
        if (p == Binding::Purpose::GetLabel)
            return ButtonClass::setButtonText (value);
    }
    
    void getComponentState (const Binding::Purpose& p, bool& value) override
    {
        if (p == Binding::Purpose::SetSelection || p == Binding::Purpose::SetValue)
        {
            value = ButtonClass::getToggleState();
            return;
        }
        UIAdaptor::getComponentState (p, value);
    }
    
    void setComponentState (const Binding::Purpose& p, bool value) override
    {
        if (p == Binding::Purpose::GetSelection || p == Binding::Purpose::GetValue)
        {
            ButtonClass::setToggleState (value, dontSendNotification);
            return;
        }
        UIAdaptor::setComponentState (p, value);
    }
    
    void clicked() override { performBinding (Binding::Purpose::Action); };
};

typedef UIButton<TextButton> UITextButton;
typedef UIButton<ImageButton> UIImageButton;
typedef UIButton<ToggleButton> UIToggleButton;


/**
 UIRadioButton extends ToggleButton in that it makes a group of radio buttons behave like
 a list of values, of which only one value can be selected at a time. Consequently, regarding
 selection state, UIRadioButton responds on behalf of its whole group.

 Since all buttons in a group respond identically, it is recommended to bind only one button
 per group to UIModel (the 'master'), in order to avoid multiple redundant calls to UIModel.
 */
class UIRadioButton :
        public juce::ToggleButton,
        public UIAdaptor
{
public:
    UIRadioButton (std::shared_ptr<UIInstance> instance, const RadioSpec& spec);
    
    void setComponentState (const Binding::Purpose& p, const String& value) override
    {
        if (p == Binding::Purpose::GetLabel)
            return setButtonText (value);
    }
    
    void getComponentState (const Binding::Purpose& p, var& value) override
    {
        if (p == Binding::Purpose::SetSelection || p == Binding::Purpose::SetValue)
        {
            value = getGroupSelection();
            return;
        }
        UIAdaptor::getComponentState (p, value);
    }
    
    void setComponentState (const Binding::Purpose& p, const var& value) override
    {
        if (p == Binding::Purpose::GetSelection || p == Binding::Purpose::GetValue)
        {
            setGroupSelection (value);
            return;
        }
        UIAdaptor::setComponentState (p, value);
    }
    
    void clicked() override;
    
    /**
     Whether this button has master role in the button group.
     UIModel should communicate only with the master button.
     */
    bool isMaster();
    
    /**
     Get the value currently selected in the button group.
     All buttons in the group will return the same value.
     */
    var getGroupSelection ();

    /**
     Set the current selection of the button group. This message can be sent to any member of the group.
     Obviously, there needs to be a member with that value in order to be toggled 'on'
     */
    void setGroupSelection (const var& value);
    
    /** Return the group's selection value, if this button is toggled 'on'  */
    int  getButtonValue() { return value; }

    void ensureMasterRoleSet();
    
private:
    void setMaster (bool master);
    
    template <typename LAMBDA1, typename LAMBDA2>
    void enumerateButtonGroup (LAMBDA1&& condition, LAMBDA2&& action)
    {
        if (auto* p = getParentComponent())
        {
            if (getRadioGroupId() != 0)
            {
                WeakReference<Component> deletionWatcher (this);
                for (auto* c : p->getChildren())
                {
                    if (auto b = dynamic_cast<UIRadioButton*> (c))
                    {
                        if (b->getRadioGroupId() == getRadioGroupId())
                        {
                            if (condition(b))
                                action (b);
                            if (deletionWatcher == nullptr)
                                return;
                        }
                    }
                }
            }
        }
    }
    
    const var value;
    int groupMaster;
};


/**
 UITableHeader extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UITableHeader :
        public juce::TableHeaderComponent,
        public UIAdaptor
{
public:
    UITableHeader (std::shared_ptr<UIInstance> instance, const TableHeaderSpec& spec) :
        TableHeaderComponent (),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
};


/**
 UITableList extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UITableList :
        public juce::TableListBox,
        public UIAdaptor
{
public:
    UITableList (std::shared_ptr<UIInstance> instance, const TableListSpec& spec) :
        TableListBox (spec.identifier),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
    
};


/**
 UISlider extends juce::Slider with a communication channel to UIModel
 @todo Implement all slider variants and settings !!
 */
class UISlider :
        public juce::Slider,
        public UIAdaptor,
        public juce::Slider::Listener
{
public:
    UISlider (std::shared_ptr<UIInstance> instance, const SliderSpecBase& spec);
   ~UISlider ();
    
    void sliderValueChanged (Slider* slider) override   { performBinding (Binding::Purpose::SetValue); }
    
    void getComponentState (const Binding::Purpose& p, double& value) override { value = Slider::getValue(); }
    void setComponentState (const Binding::Purpose& p, double  value) override { Slider::setValue (value); repaint(); }
};


/**
 UIGroup extends its Juce equivalent with a communication channel to UIModel
 UIGroup doubles as a composite and thus owns its child components and takes care of deleting them!
 */
class UIGroup :
        public juce::GroupComponent,
        public UIAdaptor
{
public:
    UIGroup (std::shared_ptr<UIInstance> instance, const GroupSpec& spec) :
        GroupComponent (spec.identifier),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
        setText (spec.label);
    }
    
    ~UIGroup () {}
    
    void setComponentState (const Binding::Purpose& p, const String& value) override
    {
        if (p == Binding::Purpose::GetLabel)
            return setText (value);
    }
};


/**
 UILabel extends its Juce equivalent with a communication channel to UIModel
 */
class UILabel :
        public juce::Label,
        public UIAdaptor
{
public:
    UILabel (std::shared_ptr<UIInstance> instance, const LabelSpec& spec) :
        Label (spec.identifier),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
        if (!spec.label.isEmpty())
            setText (spec.label, dontSendNotification);
    }
    
    void setComponentState (const Binding::Purpose& p, const String& value) override
    {
        if (p == Binding::Purpose::GetLabel || p == Binding::Purpose::GetValue)
            return setText (value, dontSendNotification);
    }
};


/**
 UIComboBox extends its Juce equivalent with a communication channel to UIModel
 */
class UIComboBox :
        public juce::ComboBox,
        public UIAdaptor
{
public:
    UIComboBox (std::shared_ptr<UIInstance> instance, const ComboSpecBase& spec);
   ~UIComboBox ();
    
    void getComponentState (const Binding::Purpose& p, String& value) override;
    void getComponentState (const Binding::Purpose& p, int& value) override;
    
    void setComponentState (const Binding::Purpose& p, UIValueMap& value) override;
    void setComponentState (const Binding::Purpose& p, const String& value) override;
    void setComponentState (const Binding::Purpose& p, int value) override;
    
private:
    void selectionChanged();
};


/**
 UIConcertina extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UIConcertina :
        public juce::ConcertinaPanel,
        public UIAdaptor
{
public:
    UIConcertina (std::shared_ptr<UIInstance> instance, const ConcertinaSpec& spec) :
        ConcertinaPanel (),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
    
};


/**
 UIToolbar extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UIToolBar :
        public juce::Toolbar,
        public UIAdaptor
{
public:
    UIToolBar (std::shared_ptr<UIInstance> instance, const ToolBarSpec& spec) :
        Toolbar (),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
    
};


/**
 UIMenuBar extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UIMenuBar :
        public juce::MenuBarComponent,
        public UIAdaptor
{
public:
    UIMenuBar (std::shared_ptr<UIInstance> instance, const MenuBarSpec& spec) :
        MenuBarComponent (),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
    
};


/**
 UIImage extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UIImage :
        public juce::ImageComponent,
        public UIAdaptor
{
public:
    UIImage (std::shared_ptr<UIInstance> instance, const ImageSpec& spec) :
        ImageComponent (spec.identifier),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
    
};



/**
 UIImagePreview extends its Juce equivalent with a communication channel to UIModel
 @todo Implement !!
 */
class UIImagePreview :
        public juce::ImagePreviewComponent,
        public UIAdaptor
{
public:
    UIImagePreview (std::shared_ptr<UIInstance> instance, const ImagePreviewSpec& spec) :
        ImagePreviewComponent (),
        UIAdaptor (instance, spec)
    {
        initialiseFromSpec (instance, spec);
    }
    
};


/**
 UIUserDefinedComponent acts as a proxy for an arbitrary component supplied by user code.
 That custom component is responsibe for taking ownership of all its children and delete
 those when it gets deleted!

 @todo: Currently this is a shell that holds the user component as its sole child. This
 offers the opportunity to use UIAdaptor for dynamic labeling, layout, etc. whatever
 a generic Component supports.
 */
class UIUserDefinedComponent :
        public juce::Component,
        public UIAdaptor
{
public:
    UIUserDefinedComponent (std::shared_ptr<UIInstance> instance, const UserDefinedSpec& spec) :
        Component (spec.identifier),
        UIAdaptor (instance, spec)
    {
       initialiseFromSpec (instance, spec);
        
        if (auto comp = spec.constructionFunct())
            addComponent (std::move(comp), LayoutFrame::entire());
    }
    
    ~UIUserDefinedComponent () {}

};

}
