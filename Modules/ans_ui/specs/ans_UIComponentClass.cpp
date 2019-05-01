/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_UIComponentClass.h"

namespace ans {
    using namespace juce;

using EC = UIComponentClass;
const Array<UIComponentClass> UIComponentClass::KnownClasses = {
    EC (EC::Type::Undefined,    "Undefined", {}),
    EC (EC::Type::Window,       "Window", {}),
    EC (EC::Type::Dialog,       "Dialog", {}),
    EC (EC::Type::Button,       "Button", {}),
    EC (EC::Type::Radio,        "Radio", {}),
    EC (EC::Type::Toggle,       "Toggle", {}),
    EC (EC::Type::Label,        "Label", {}),
    EC (EC::Type::Input,        "Input", {}),
    EC (EC::Type::Text,         "Text", {}),
    EC (EC::Type::Code,         "Code", {}),
    EC (EC::Type::Combo,        "Combo", {}),
    EC (EC::Type::Popup,        "Popup", {}),
    EC (EC::Type::List,         "List", {}),
    EC (EC::Type::Tree,         "Tree", {}),
    EC (EC::Type::TableHeader,  "TableHeader", {}),
    EC (EC::Type::TableList,    "TableList", {}),
    EC (EC::Type::Progress,     "Progress", {}),
    EC (EC::Type::Slider,       "Slider", {}),
    EC (EC::Type::Knob,         "Knob", {}),
    EC (EC::Type::Entry,        "Entry", {}),
    EC (EC::Type::Composite,    "Composite", {}),
    EC (EC::Type::Group,        "Group", {}),
    EC (EC::Type::Tabs,         "Tabs", {}),
    EC (EC::Type::Concertina,   "Concertina", {}),
    EC (EC::Type::MenuBar,      "MenuBar", {}),
    EC (EC::Type::ToolBar,      "ToolBar", {}),
    EC (EC::Type::Image,        "Image", {}),
    EC (EC::Type::ImagePreview, "ImagePreview", {}),
    EC (EC::Type::Canvas,       "Canvas", {}),
    EC (EC::Type::UserDefined,  "UserDefined", {})
};

UIComponentClass::UIComponentClass (Type enumKey, const String& xmlName, const StringArray& xmlProperties) :
    type (enumKey),
    name (xmlName),
    properties (xmlProperties)
{
//   if (!KnownClasses.contains(*this))
//       KnownClasses.add (*this);
}

const UIComponentClass& UIComponentClass::get (Type t)
{
    auto& result = KnownClasses.getReference((int)t);
    // Check if array is indexed correctly. If you get a assertion here, there may be gaps or sequence doesn't match
    jassert (result.type == t);
    return result;
}

const UIComponentClass& UIComponentClass::parse (const String& name)
{
    for (auto& t : UIComponentClass::KnownClasses)
        if (t.name == name)
            return t;
    
    DBG ("*** ERROR: Unknown UIComponentClass " << name.quoted());
    jassertfalse;
    return UIComponentClass::get (Type::Undefined);
}


}//

