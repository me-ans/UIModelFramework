/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

/**
 ComponentSpec, WindowSpec, its subclasses and closely related classes ("specs") represent the layout
 and data bindings of a UI in native format. UIBuilder takes a WindowSpec to configure a window and
 populate it with components. Specs are compiled and supplied to a UIBuilder that builds a Juce UI
 accordingly. Specs can also be provided to a UI Editor and saved back to C++ source code in order to
 */

#pragma once

#include "../core/ans_Binding.h"
#include "../core/ans_Selection.h"
#include "../utility/ans_LayoutFrame.h"
#include "../utility/ans_SourceOutputStream.h"
#include "../specs/ans_UIComponentClass.h"

namespace ans {
    using namespace juce;
/**
 Juce 5 happens to use String for component ids. Eventually this should be changed to Identifier.
 */
typedef String ComponentID;

class UIModel;
class UISpec;
class ComponentSpec;
class ComponentSpecInspector;


/**
 UIValueMap is used to supply components with a set of key-value pairs, e.g. for a menu
 */
typedef Array<std::pair<int,String>> UIValueMap;

/**
 LayoutSpec holds all parameters required to create a layout object that
 computes the relative position of a component. Currently FlexBox and LayoutFrame are supported.
 */
struct LayoutSpec
{
    typedef enum
    {
        Frame = 0,
        Flex
    } Type;
    
    /** Labels used for generating code and parsing input */
    static const StringArray TypeNames;
    
    /** Convenience: Proportional layout that fills entire parent area */
    static LayoutSpec entire ();
    
    LayoutSpec ();
    LayoutSpec (const LayoutFrame& f) : type(Frame), frame(f) {}
   ~LayoutSpec () {}
    
    /**  Attach an appropriate Positioner to the Component, according to the spec's properties */
    void applyToComponent (Component* comp) const;
    
    Type type = Type::Frame;
    LayoutFrame frame;
    juce::FlexItem item;    // Placement in parent FlexBox, if any
    juce::FlexBox box;      // Placement rules for children, if any
};

//=====================================================================================================

/**
 ColourSpec specifies any number of colour assignments to a Component.
 ColourSpec::List is a handy argument for updating select colours dynamically.
 */
struct ColourSpec
{
    /** Pairs a colour with its source code for code (re)gerenation */
    struct Item
    {
        Item (int i, const Colour& c, const String& s) : identifier(i), colour(c), source(s) {}
        Item (int i, const Colour& c) : identifier(i), colour(c), source("0x" + c.toString()) {}
        
        String generateSourceCPP() const;
        
        int identifier;
        Colour colour;
        String source;
    };
    
    typedef Array<Item> List;
    
    ColourSpec& add (int colourID, const Colour& colour);
    ColourSpec& add (int colourID, const Colour& colour, const String& source);
    
    List assignments;
};

//=====================================================================================================

/**
 ComponentSpec is the base class of specs for components of various types. It holds all parameters
 required to create a Component. To keep things simple, this struct is basically a flat union of
 parameters needed for a variety of subclasses of Component. It is the responsibility of UIBuilder
 to pick the parameters it needs for a particular Component.
 */

class ComponentSpec : public UIItemInterface
{
public:
    
    METACLASS_ROOT_BEGIN (ComponentSpec)
    
    /**
     Get the class name of a subclass of ComponentSpecInspector, which is appropriate for editing
     instances of this class. Note: Since ComponentSpecInspector::Class cannot be forward declared,
     we need to look it up by name instead.
     */
    virtual const String getInspectorClassName () { return "ComponentSpecInspector"; }
    
    /** A user-friendly label for use with UIEditor */
    virtual const String getUserLabel () { return getName().replace("Spec", ""); }
    
    /** Supply a new default instance of this class for use in the ComponentSpecInspector */
    virtual ComponentSpec* newFor (ComponentSpecInspector* inspector);
    
    METACLASS_ROOT_END
    

    ComponentSpec (UIComponentClass::Type t, const ComponentID& n, Aspect a = Model::Undefined) :
        type (t),
        identifier (n),
        layout (LayoutFrame::entire()),
        aspect (a)
    {}
    
    virtual ~ComponentSpec() {}
    
    /** Render C++ source for the *.specs.cpp file */
    String generateSourceCPP (Model::Class* modelClass) const;
    void   generateSourceCPP (Model::Class* modelClass, SourceOutputStream& source, const ComponentSpec* parent = nullptr) const;
    
    /** Render an expression like 'auto name = new WhateverSpec ();' that is appropriate for the spec */
    virtual void generateSourceCreation (Model::Class* modelClass, SourceOutputStream& out) const;

    /** Render subclass-specific properties as C++ expressions for the *.specs.cpp file */
    virtual void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const;
    
    /** Collect all aspects used by the component and all its children */
    Aspects getUsedAspects () const;
    
    /** Make the component use a Positioner based on the given LayoutFrame */
    void setLayout (const LayoutFrame& frame) { layout = LayoutSpec (frame); }
    
    /** Set whatever the component considers its label */
    void setLabel (const String& l) { label = l; }
    
    /** Set a generic ARGB colour */
    void setColour (int identifier, const Colour& colour) { colours.add (identifier, colour); }
    
    /** Set a colour along with the source code for code (re)gerenation */
    void setColour (int identifier, const Colour& colour, const String& source) { colours.add (identifier, colour, source); }
    
    /** Set the default aspect for bindings that require one but don't bother (this can be passed to the constructor already) */
    void setAspect (Aspect a) { aspect = a; }
    
    /** Add a component to the spec as a child and take ownership of it */
    ComponentSpec* addComponent (ComponentSpec* comp) { children.add(comp); return comp; }

    /** The spec takes ownership of the Binding */
    void addBinding (Binding* binding);
    
    /** Return true if this is a WindowSpec */
    virtual bool isWindowSpec() { return false; }
    
    /** Return true if the component can be populated with children that it will take ownership of */
    virtual bool isComposite() { return false; }
    
    // ================= UIItemInterface Protocol ===============================
    
    const String getItemString() const override { return getClass()->getUserLabel(); }
    
    const String getItemIdentifier() override { return identifier; }
    
    bool itemHasChildren() override { return ! children.isEmpty(); }
    
    Array<UIItemInterface*> getItemChildren() override { return UIItemInterface::convert (children); }
    
    // ==========================================================================

    
    UIComponentClass::Type type;
    ComponentID identifier;
    String label;
    String tooltip;
    int focusOrder = 0;
    LayoutSpec layout;
    ColourSpec colours;
    Aspect aspect = Model::Undefined;
    OwnedArray<Binding> bindings;
    OwnedArray<ComponentSpec> children;
    
private:
    JUCE_LEAK_DETECTOR (ComponentSpec)
};

//=====================================================================================================

/** Base class for the variety of button specs */
class ButtonSpecBase : public ComponentSpec
{
public:
    /** Labels used for generating code and parsing Button::ConnectedEdgeFlags */
    static const StringArray ConnectionNames;
    
    METACLASS_BEGIN (ButtonSpecBase, ComponentSpec)
    METACLASS_END
    
    ButtonSpecBase (UIComponentClass::Type t,
                    const ComponentID& n,
                    Aspect a = Model::Undefined) :
        ComponentSpec (t,n,a)
    {}
    
    void setGroup (int g) { groupId = g; }
    void setValue (const var& v) { groupValue = v; }
    void setConnectedEdges (int flags) { groupConnections = flags; }
    
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    int groupId = 0;
    var groupValue;
    int groupConnections = 0;
    
};

//=====================================================================================================

/** Spec for radio buttons */
class ButtonSpec : public ButtonSpecBase
{
public:
    
    METACLASS_BEGIN (ButtonSpec, ButtonSpecBase)
    METACLASS_END
    
    ButtonSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ButtonSpecBase (UIComponentClass::Type::Button, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for radio buttons */
class RadioSpec : public ButtonSpecBase
{
public:
    
    METACLASS_BEGIN (RadioSpec, ButtonSpecBase)
    METACLASS_END
    
    RadioSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ButtonSpecBase (UIComponentClass::Type::Radio, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for toggle buttons (check boxes) */
class ToggleSpec : public ButtonSpecBase
{
public:
    
    METACLASS_BEGIN (ToggleSpec, ButtonSpecBase)
    METACLASS_END
    
    ToggleSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ButtonSpecBase (UIComponentClass::Type::Toggle, n, a)
    {}
    
    
    bool initialState = false;
};

//=====================================================================================================

/** Base class for the variety of slider specs */
class SliderSpecBase : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (SliderSpecBase, ComponentSpec)
    METACLASS_END
    
    SliderSpecBase (UIComponentClass::Type t,
                    const ComponentID& n,
                    Aspect a = Model::Undefined) :
        ComponentSpec (t,n,a)
    {}
    
    void setRange (double min, double max, double interval) { valueRange = {min,max}; valueInterval = interval; }
    
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    Range<double> valueRange = Range<double>(0,1);
    double valueInterval = 0.1;
};

//=====================================================================================================

/** Spec for linear sliders */
class SliderSpec : public SliderSpecBase
{
public:
    
    METACLASS_BEGIN (SliderSpec, SliderSpecBase)
    METACLASS_END
    
    SliderSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        SliderSpecBase (UIComponentClass::Type::Slider, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for rotary sliders */
class KnobSpec : public SliderSpecBase
{
public:
    
    METACLASS_BEGIN (KnobSpec, SliderSpecBase)
    METACLASS_END
    
    KnobSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        SliderSpecBase (UIComponentClass::Type::Knob, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for inc/dec value entries */
class EntrySpec : public SliderSpecBase
{
public:
    
    METACLASS_BEGIN (EntrySpec, SliderSpecBase)
    METACLASS_END
    
    EntrySpec (const ComponentID& n, Aspect a = Model::Undefined) :
        SliderSpecBase (UIComponentClass::Type::Entry, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for labels */
class LabelSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (LabelSpec, ComponentSpec)
    METACLASS_END
    
    LabelSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::Label, n, a)
    {}
    
    
    Font font;
    bool editOnSingleClick = false;
    bool editOnDoubleClick = false;
    bool lossOfFocusDiscardsChanges = true;
    Justification justification = Justification::centredLeft;
};

//=====================================================================================================

/** Spec for text editors of different types */
class TextSpecBase : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (TextSpecBase, ComponentSpec)
    METACLASS_END
    
    TextSpecBase (UIComponentClass::Type t, const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (t, n, a)
    {}
    
    bool enableMultiLine = false;
    bool enableEditing = true;
    bool enableScrollbars = true;
    bool enableCaret = true;
    bool enablePopupMenu = true;
    
    String initialText;
};

//=====================================================================================================

/** Spec for multi-line text editors */
class TextSpec : public TextSpecBase
{
public:
    
    METACLASS_BEGIN (TextSpec, TextSpecBase)
    METACLASS_END
    
    TextSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        TextSpecBase (UIComponentClass::Type::Text, n, a)
    {
        enableMultiLine = true;
    }
    
    
};

//=====================================================================================================

/** Spec for single-line input fields */
class InputSpec : public TextSpecBase
{
public:
    
    METACLASS_BEGIN (InputSpec, TextSpecBase)
    METACLASS_END
    
    InputSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        TextSpecBase (UIComponentClass::Type::Input, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for multi-line source code editors */
class CodeSpec : public TextSpecBase
{
public:
    
    METACLASS_BEGIN (CodeSpec, TextSpecBase)
    METACLASS_END
    
    CodeSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        TextSpecBase (UIComponentClass::Type::Code, n, a)
    {
        enableMultiLine = true;
    }
    
};

//=====================================================================================================

/** Base spec for combo boxes of different types */
class ComboSpecBase : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ComboSpecBase, ComponentSpec)
    METACLASS_END
    
    ComboSpecBase (UIComponentClass::Type t, const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (t, n, a)
    {}
    
    StringArray initialItems;
    String defaultUnselectedText;
    String defaultNoChoicesText;
    bool enableEditing = false;
    Justification justification = Justification::centredLeft;
};

//=====================================================================================================

/** Spec for popup menu buttons */
class PopupSpec : public ComboSpecBase
{
public:
    
    METACLASS_BEGIN (PopupSpec, ComboSpecBase)
    METACLASS_END
    
    PopupSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComboSpecBase (UIComponentClass::Type::Popup, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for combo boxes */
class ComboSpec : public ComboSpecBase
{
public:
    
    METACLASS_BEGIN (ComboSpec, ComboSpecBase)
    METACLASS_END
    
    ComboSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComboSpecBase (UIComponentClass::Type::Combo, n, a)
    {
        enableEditing = true;
    }
    
};


//=====================================================================================================

/** Spec for UIListBox, where the ListBoxModel is provided by UIModel */
class ListSpecBase : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ListSpecBase, ComponentSpec)
    METACLASS_END
    
    ListSpecBase (UIComponentClass::Type t, const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (t, n, a)
    {}    
    
    void setMultipleSelectionEnabled (bool enable) { enableMultipleSelection = enable; }
    
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    bool enableMultipleSelection = false;
};

//=====================================================================================================

/** Spec for UIListBox, where the ListBoxModel is provided by UIModel */
class ListSpec : public ListSpecBase
{
public:
    
    METACLASS_BEGIN (ListSpec, ListSpecBase)
    METACLASS_END
    
    ListSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ListSpecBase (UIComponentClass::Type::List, n, a)
    {}
};


//=====================================================================================================

/** Spec for UITreeView */
class TreeSpec : public ListSpecBase
{
public:
    
    METACLASS_BEGIN (TreeSpec, ListSpecBase)
    METACLASS_END
    
    TreeSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ListSpecBase (UIComponentClass::Type::Tree, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for UITableHeader */
class TableHeaderSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (TableHeaderSpec, ComponentSpec)
    METACLASS_END
    
    TableHeaderSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::TableHeader, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for UITable */
class TableListSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (TableListSpec, ComponentSpec)
    METACLASS_END
    
    TableListSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::TableList, n, a)
    {}
};

//=====================================================================================================

/** Spec for UIProgressBar */
class ProgressSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ProgressSpec, ComponentSpec)
    METACLASS_END
    
    ProgressSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::Progress, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for UIGroup */
class GroupSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (GroupSpec, ComponentSpec)
    METACLASS_END
    
    GroupSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::Group, n, a)
    {}
    
    bool isComposite() override { return true; }
    
    Justification justification = Justification::centredLeft;
};

//=====================================================================================================

/** Base spec for different types of composite components */
class CompositeSpecBase : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (CompositeSpecBase, ComponentSpec)
    METACLASS_END
    
    CompositeSpecBase (UIComponentClass::Type t, const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (t, n, a)
    {}
    
    bool isComposite() override { return true; }
    
    void setBackgroundColour (Colour colour) { backgroundColour = {0, colour}; }
    void setBackgroundColour (Colour colour, const String& source) { backgroundColour = {0, colour, source}; }
    
    bool hasBackgroundColour() const { return backgroundColour.colour != Colours::transparentBlack; }
    
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    ColourSpec::Item backgroundColour = {0, Colours::transparentBlack};

};

//=====================================================================================================

/** Spec for UIComposite */
class CompositeSpec : public CompositeSpecBase
{
public:
    
    METACLASS_BEGIN (CompositeSpec, CompositeSpecBase)
    METACLASS_END
    
    CompositeSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        CompositeSpecBase (UIComponentClass::Type::Composite, n, a)
    {}
};

//=====================================================================================================

/**
 Spec for an empty composite shell populated by UIModel on request
 */
class CanvasSpec : public CompositeSpecBase
{
public:
    
    METACLASS_BEGIN (CanvasSpec, CompositeSpecBase)
    METACLASS_END
    
    CanvasSpec (const ComponentID& n) :
        CompositeSpecBase (UIComponentClass::Type::Canvas, n)
    {}
    
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    int customType = 0;
};

//=====================================================================================================

/**
 Spec for arbitrary components that are created by user code
 */
class UserDefinedSpec : public ComponentSpec
{
public:
    typedef std::function<Component*()> ConstructorFunct;
    
    METACLASS_BEGIN (UserDefinedSpec, ComponentSpec)
    METACLASS_END
    
    
    UserDefinedSpec (const ComponentID& n, const String& source, ConstructorFunct fun) :
        ComponentSpec (UIComponentClass::Type::UserDefined, n),
        constructionSource (source),
        constructionFunct (fun)
    {}
    
    void generateSourceCreation (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    String constructionSource;
    ConstructorFunct constructionFunct;
};





//=====================================================================================================

/** Spec for UIConcertina */
class ConcertinaSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ConcertinaSpec, ComponentSpec)
    METACLASS_END
    
    ConcertinaSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::Concertina, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for UIMenuBar */
class MenuBarSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (MenuBarSpec, ComponentSpec)
    METACLASS_END
    
    MenuBarSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::MenuBar, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for UIToolBar */
class ToolBarSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ToolBarSpec, ComponentSpec)
    METACLASS_END
    
    ToolBarSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::ToolBar, n, a)
    {}
    
};

//=====================================================================================================

/** Spec for UIImage */
class ImageSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ImageSpec, ComponentSpec)
    METACLASS_END
    
    ImageSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::Image, n, a)
    {}
    
};


//=====================================================================================================

/** Spec for UIImagePreview */
class ImagePreviewSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (ImagePreviewSpec, ComponentSpec)
    METACLASS_END
    
    ImagePreviewSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        ComponentSpec (UIComponentClass::Type::ImagePreview, n, a)
    {}

};


//=====================================================================================================

/**
 Top-level spec for a variety of window types
 */
class WindowSpec : public ComponentSpec
{
public:
    
    METACLASS_BEGIN (WindowSpec, ComponentSpec)
    METACLASS_END
    
    WindowSpec (const ComponentID& n = "window", UIComponentClass::Type t = UIComponentClass::Type::Window);
    
    void generateSourceCreation   (Model::Class* modelClass, SourceOutputStream& out) const override;
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    bool isComposite() override { return true; }
    bool isWindowSpec() override { return true; }
    
    
    bool enableNativeTitleBar = true;
};


} // namespace
