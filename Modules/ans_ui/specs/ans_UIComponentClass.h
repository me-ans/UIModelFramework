/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "JuceHeader.h"

namespace ans {
    using namespace juce;

/**
 UIComponentClass represents different types of UI elements. These do not map 1:1 to juce::Component,
 which is also why they are named differently. For some juce::Component there are multiple UIComponentClass,
 e.g. 'InputField' is a single-line TextEditor and 'RadioButton' and 'Toggle' both map to ToggleButton.
 Shorter, purpose-oriented names make a UI's spec more readable.
 
 Technically, this class implements kind of an 'intelligent' enum, with a focus on reflection and
 easy maintenance and extension.
 */

struct UIComponentClass
{
    enum class Type
    {
        Undefined = 0,
        // Windows
        Window,             // Top-level window on desktop
        Dialog,             // Top-level dialog window
        // Buttons
        Button,             // TextButton, or ImageButton depending on properties
        Radio,              // ToggleButton button with list-like selection protocol
        Toggle,             // ToggleButton with sensible defaults for use as a checkbox
        // Text
        Label,
        Input,              // TexEditor with sensible defaults for a single-line input fields
        Text,               // TexEditor with sensible defaults for a multi-line text
        Code,               // Source code editor
        // Menus
        Combo,              // ComboBox
        Popup,              // ComboBox with a popup menu only (no text input)
        // Lists & Tables
        List,               // Full list with UIListModel provided by UIModel
        Tree,
        TableHeader,
        TableList,
        // Continuous Controls
        Progress,           // ProgressBar
        Slider,             // Slider with sensible defaults for a linear appearance
        Knob,               // Slider with sensible defaults for a rotary appearance
        Entry,              // Slider with sensible defaults for the inc/dec style
        // Composites
        Composite,          // Groups multiple components for layout purposes
        Group,              // Groups multiple components for layout purposes, with a label and frame
        Tabs,
        Concertina,
        MenuBar,
        ToolBar,
        // Graphics
        Image,
        ImagePreview,
        // Application-specific
        Canvas,             // Composite component populated by UIModel on request
        UserDefined         // Arbitrary component supplied by user code
    };
    
    UIComponentClass (Type typeEnum, const String& xmlName, const StringArray& xmlProperties);
    
    /** Converts to int, so it can be used as an index into Array, etc. */
    operator int() const { return static_cast<int>(type); }

    bool operator== (const UIComponentClass& c) const noexcept { return type == c.type; }
    
    bool isUndefined() const { return type == Type::Undefined; }
    
    /** Get a reference to a global UIComponentClass by name */
    static const UIComponentClass& parse (const String& n);
    
    /** Get a reference to a global UIComponentClass by type enum */
    static const UIComponentClass& get (Type t);
    
    static const Array<UIComponentClass> KnownClasses;
    
    //Member Variables
    Type type;
    String name;
    StringArray properties;
};

}



