/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_ComponentSpec.h"
#include "../utility/ans_Positioners.h"
#include "../core/ans_UIBuilder.h"
#include "../core/ans_UIInstance.h"
#include "../editor/inspector/ans_ComponentSpecInspector.h"

namespace ans {
    using namespace juce;
    
    namespace EnumHelpers
    {
        template <typename Enum>
        Enum parse (const StringArray& names, const String& s)
        {
            int i = names.indexOf(s, true);
            if (i < 0)
            {
                DBG ("*** ERROR: Unknown constant " << s.quoted());
                return (Enum)0;
            }
            return (Enum)i;
        }
        
        template <typename Bitmap>
        Bitmap parseFlags (const StringArray& names, const String& s)
        {
            auto tokens = StringArray::fromTokens (s, false);
            // individual tokens
            uint64 bits = 0;
            for (auto t : tokens)
            {
                int i = names.indexOf(t, true);
                if (i >= 0)
                    bits |= (1<<i);
            }
            return static_cast<Bitmap>(bits);
        }
        
        String print (const StringArray& names, int i)
        {
            return names[i];
        }
        
        String printFlags (const StringArray& names, int flags)
        {
            String output;
            for (int o = 0; o < names.size(); ++o)
                if ((flags & (1<<o)) > 0)
                {
                    if (!output.isEmpty()) output << " | ";
                    output << names[o];
                }
            return output;
        }
    }
    
    //==========================================================================================================
#if 0
#pragma mark ColourSpec
#endif
    
    ColourSpec& ColourSpec::add (int colourID, const Colour& colour)
    {
        assignments.add (Item(colourID, colour));
        return *this;
    }
    
    ColourSpec& ColourSpec::add (int colourID, const Colour& colour, const String& source)
    {
        assignments.add (Item(colourID, colour, source));
        return *this;
    }
    
    String ColourSpec::Item::generateSourceCPP() const
    {
        if (source.isNotEmpty())
            return "SOURCE (" + source + ")";
        else
            return "0x" + colour.toString().toUpperCase();
    }
    
    //==========================================================================================================
#if 0
#pragma mark LayoutSpec
#endif
    
    const StringArray LayoutSpec::TypeNames = {
        "Frame",
        "Flex"
    };
    
    LayoutSpec::LayoutSpec () :
    type (Frame),
    frame ()
    {
        flexBox.flexDirection = FlexBox::Direction::row;
        flexBox.alignItems = FlexBox::AlignItems::flexStart;
        flexBox.flexWrap = FlexBox::Wrap::wrap;
        /** @todo: find sensible default parameters */
    }
    
    LayoutSpec LayoutSpec::entire ()
    {
        LayoutSpec spec;
        spec.type = Frame;
        spec.frame.right.fraction = 1.0;
        spec.frame.bottom.fraction = 1.0;
        return spec;
    }
    
    void LayoutSpec::applyToComponent (Component* comp) const
    {
        if (comp == nullptr)
            return;
        
        switch (type)
        {
            case LayoutSpec::Type::Frame:
            {
                comp->setPositioner (new FramePositioner (*comp, frame));
                break;
            }
            case LayoutSpec::Type::Grid:
            {
                comp->setPositioner (new GridPositioner (*comp, grid, gridItem));
                break;
            }
            case LayoutSpec::Type::Flex:
            {
                comp->setPositioner (new FlexPositioner (*comp, flexBox, flexItem));
                break;
            }
            default:
                DBG ("*** ERROR: Unsupported layout type specified: " << (int)type);
                comp->setPositioner (new FramePositioner (*comp, LayoutFrame::entire()));
                break;
        }
    }
    
    //==========================================================================================================
#if 0
#pragma mark ComponentSpec
#endif
    
    ComponentSpec* ComponentSpec::Class::newFor (ComponentSpecInspector* inspector)
    {
        return new ComponentSpec (UIComponentClass::Type::Composite, inspector->getSuggestedID ("composite"));
    }
    
    std::unique_ptr<Component> ComponentSpec::buildInstance (std::shared_ptr<UIInstance> instance) const
    {
        DBG ("*** ERROR: Unsupported or not yet implemented component type: " << (int)(type));
        jassertfalse;
        return nullptr;
    }
    
    void ComponentSpec::addBinding (Binding* binding)
    {
        jassert (binding != nullptr && !binding->isBound());
        
        // Ensure an aspect, if necessary
        if (binding->isAspected() && binding->getAspect() == Model::Undefined)
        {
            if (aspect != Model::Undefined)
            {
                // The the spec has an aspect, its bindings inherit it by default
                binding->setAspect (aspect);
            }
            else if (binding->purpose.defaultAspect != Model::Undefined)
            {
                binding->setAspect (binding->purpose.defaultAspect);
                
                if (binding->purpose.userDefinedAspectRequired)
                {
                    DBG ("*** WARNING: Binding " << binding->purpose.name << " for component "
                         << identifier.quoted()
                         << " requires an aspect - assuming default "
                         << Symbol(binding->getAspect()).toString().quoted());
                }
            }
        }
        bindings.add (binding);
    }
    
    String ComponentSpec::generateSourceCPP (Model::Class* modelClass) const
    {
        SourceOutputStream out;
        out.incrementIndent();
        out << newLine;
        generateSourceCPP (modelClass, out);
        out.decrementIndent();
        out << "return " << identifier << ";";
        out.decrementIndent();
        out << newLine;
        return out.toString();
    }
    
    void ComponentSpec::generateSourceCPP (Model::Class* modelClass, SourceOutputStream& out, const ComponentSpec* parent) const
    {
        generateSourceCreation (modelClass, out);
        
        if (layout.type == LayoutSpec::Frame && LayoutFrame::entire() != layout.frame)
            out << identifier << "->setLayout (" << layout.frame.generateSourceCPP() << ");" << newLine;
        
        if (label.isNotEmpty())
            out << identifier << "->setLabel (" << label.quoted() << ");" << newLine;
        
        for (auto c : colours.assignments)
            out << identifier << "->setColour (0x" << String::toHexString(c.identifier).toUpperCase() << ", " << c.generateSourceCPP() << ");" << newLine;
        
        generateSourceProperties (modelClass, out);
        
        for (auto b : bindings)
            out << identifier << "->addBinding (" << b->generateSourceCPP (modelClass) << ");" << newLine;
        
        for (auto& c : children)
        {
            out << "{";
            out.incrementIndent();
            out << newLine;
            c->generateSourceCPP (modelClass, out, this);
            out.decrementIndent();
            out << newLine << "}" << newLine;
        }
        
        if (parent != nullptr)
            out << parent->identifier << "->addComponent (" << identifier << ");";
    }
    
    void ComponentSpec::generateSourceCreation (Model::Class* modelClass, SourceOutputStream& out) const
    {
        out << "auto " << identifier << " = new " << getClass()->getName() << " (" << identifier.quoted();
        
        if (aspect != Model::Undefined)
            out << ", " << modelClass->printAspect(aspect);
        
        out << ");" << newLine;
    }
    
    void ComponentSpec::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
    }
    
    Aspects ComponentSpec::getUsedAspects () const
    {
        Aspects answer;
        
        if (aspect != Model::Undefined)
            answer.add (aspect);
        
        for (auto bind : bindings)
            if (bind != nullptr && bind->getAspect() != Model::Undefined)
                answer.add (bind->getAspect());
        
        for (auto& child : children)
            for (auto a : child->getUsedAspects())
                answer.add (a);
        
        return answer;
    }
    
    
    //==========================================================================================================
#if 0
#pragma mark CompositeSpecBase
#endif
    
    void CompositeSpecBase::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
        if (hasBackgroundColour())
            out << identifier << "->setBackgroundColour (" << backgroundColour.generateSourceCPP() << ");" << newLine;
    }
    
    //==========================================================================================================
#if 0
#pragma mark SliderSpecBase
#endif
    
    void SliderSpecBase::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
        out << identifier << "->setRange ("
        << valueRange.getStart() << ", "
        << valueRange.getEnd() << ", "
        << valueInterval << ");" << newLine;
    }
    
    //==========================================================================================================
#if 0
#pragma mark ButtonSpecBase
#endif
    
    const StringArray ButtonSpecBase::ConnectionNames = {
        "Button::ConnectedOnLeft",
        "Button::ConnectedOnRight",
        "Button::ConnectedOnTop",
        "Button::ConnectedOnBottom"
    };
    
    void ButtonSpecBase::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
        if (groupId != 0)
            out << identifier << "->setGroup (" << groupId << ");" << newLine;
        
        if (type == UIComponentClass::Type::Radio)
            out << identifier << "->setValue (" << groupValue.toString() << ");" << newLine;
        
        if (groupConnections != 0)
        {
            out << identifier << "->setConnectedEdges ("
            << EnumHelpers::printFlags (ConnectionNames, groupConnections)
            << ");"
            << newLine;
        }
    }
    
    
    //==========================================================================================================
#if 0
#pragma mark UserDefinedSpec
#endif
    
    void UserDefinedSpec::generateSourceCreation (Model::Class* modelClass, SourceOutputStream& out) const
    {
        out << "auto " << identifier << " = new " << getClass()->getName() << " ("
        << identifier.quoted() << ", " << constructionSource.quoted()
        << ", []() { return new " << constructionSource << "; });"
        << newLine;
    }
    
    //==========================================================================================================
#if 0
#pragma mark CanvasSpec
#endif
    
    void CanvasSpec::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
    }
    
    //==========================================================================================================
#if 0
#pragma mark ListSpecBase
#endif
    
    void ListSpecBase::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
        if (enableMultipleSelection)
            out << identifier << "->setMultipleSelectionEnabled (true);" << newLine;
    }
    
    
    //==========================================================================================================
#if 0
#pragma mark WindowSpec
#endif
    
    WindowSpec::WindowSpec (const ComponentID& n, UIComponentClass::Type t) :
    ComponentSpec (t, n)
    {
    }
    
    void WindowSpec::generateSourceCreation (Model::Class* modelClass, SourceOutputStream& out) const
    {
        out << "auto " << identifier << " = new " << getClass()->getName() << " (";
        
        if (identifier != "window" || type != UIComponentClass::Type::Window)
            out << identifier.quoted();
        
        if (type != UIComponentClass::Type::Window)
            out << ", UIComponentClass::Type::" << UIComponentClass::get(type).name;
        
        out << ");" << newLine;
    }
    
    void WindowSpec::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
    {
    }
    
    
    //==========================================================================================================
#if 0
#pragma mark Implementations of buildInstance()
#endif
    
    std::unique_ptr<Component> ButtonSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITextButton> (instance, *this); }
    std::unique_ptr<Component> ToggleSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIToggleButton> (instance, *this); }
    std::unique_ptr<Component> RadioSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIRadioButton> (instance, *this); }
    std::unique_ptr<Component> LabelSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UILabel> (instance, *this); }
    std::unique_ptr<Component> InputSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITextEditor> (instance, *this); }
    std::unique_ptr<Component> TextSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITextEditor> (instance, *this); }
    std::unique_ptr<Component> CodeSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UICodeEditor> (instance, *this); }
    std::unique_ptr<Component> ComboSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIComboBox> (instance, *this); }
    
    std::unique_ptr<Component> PopupSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIComboBox> (instance, *this); }
    std::unique_ptr<Component> ListSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIListBox> (instance, *this); }
    std::unique_ptr<Component> TreeSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITreeView> (instance, *this); }
    std::unique_ptr<Component> TableHeaderSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITableHeader> (instance, *this); }
    std::unique_ptr<Component> TableListSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITableList> (instance, *this); }
    std::unique_ptr<Component> ProgressSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIProgressBar> (instance, *this); }
    std::unique_ptr<Component> SliderSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UISlider> (instance, *this); }
    std::unique_ptr<Component> KnobSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UISlider> (instance, *this); }
    std::unique_ptr<Component> EntrySpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UISlider> (instance, *this); }
    std::unique_ptr<Component> CompositeSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIComposite> (instance, *this); }
    std::unique_ptr<Component> GroupSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIGroup> (instance, *this); }
    std::unique_ptr<Component> TabsSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UITabComposite> (instance, *this); }
    
    
    std::unique_ptr<Component> ConcertinaSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIConcertina> (instance, *this); }
    std::unique_ptr<Component> MenuBarSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIMenuBar> (instance, *this); }
    std::unique_ptr<Component> ToolBarSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIToolBar> (instance, *this); }
    std::unique_ptr<Component> ImageSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIImage> (instance, *this); }
    std::unique_ptr<Component> ImagePreviewSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIImagePreview> (instance, *this); }
    std::unique_ptr<Component> CanvasSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIComposite> (instance, *this); }
    std::unique_ptr<Component> UserDefinedSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { return std::make_unique<UIUserDefinedComponent> (instance, *this); }
    
    std::unique_ptr<Component> WindowSpec::buildInstance (std::shared_ptr<UIInstance> instance) const { jassertfalse; return nullptr; }
    
}//


