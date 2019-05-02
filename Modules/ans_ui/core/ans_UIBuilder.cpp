/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

//#include "ans_UIBuilder.h"
//#include "ans_UIInstance.h"
//#include "../utility/ans_Positioners.h"
//#include "../editor/ans_UIEditor.h"

namespace ans
{
    using namespace juce;
    
TopLevelWindow* UIBuilder::buildWindow (UISpec* spec, WindowUIModel* model)
{
    if (model == nullptr)
        return nullptr;

    const WindowSpec* windowSpec = dynamic_cast<const WindowSpec*>(spec->getRootSpec());
    if (windowSpec == nullptr)
    {
        DBG ("*** ERROR: WindowSpec required for building a window");
        jassertfalse;
        return nullptr;
    }

    if (spec->getModelClass() != model->getClass())
    {
        DBG ("*** ERROR: Attempt to build " << spec->getModelClass()->getName() << " for class " << model->getClass()->getName());
        jassertfalse;
    }
    
    /** @todo respect other window types */
    auto owner = new UIInstance (model, spec);
    auto window = new UIDocumentWindow (owner, *windowSpec);
    
    if (auto contentSpec = windowSpec->children.getFirst())
    {
        auto content = buildComponent (contentSpec, owner, nullptr);
        window->setContentNonOwned (content, false);
    }

    model->postBuild (*owner);
    model->changed (Model::Undefined);
    
    auto screen = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    auto area = windowSpec->layout.frame.rectangleRelativeTo (screen);
    window->setBounds (area);
    
    return window;
}

bool UIBuilder::buildInto (UISpec* spec, UIModel* model, UIComposite* composite)
{
    if (composite == nullptr)
        return false;
    
    const ComponentSpec* componentSpec = spec->getRootSpec();
    if (componentSpec == nullptr)
    {
        jassertfalse;
        return false;
    }
    
    composite->componentBuildBegin();
    auto instance = composite->getOwner()->getChildInstanceFor (model, spec);
    instance->clear();
    
    for (auto childSpec : componentSpec->children)
        buildComponent (childSpec, instance, composite);
    
    buildEditLink (instance, composite);
    composite->componentBuildEnd();
    
    model->postBuild (*instance);
    return true;
}


Component* UIBuilder::buildComponent (const ComponentSpec* spec, UIInstance* instance, Component* parent)
{
    if (spec == nullptr)
        return nullptr;
    
    Component* comp = buildComponentInstance (spec, instance, parent);
    if (comp == nullptr)
        return nullptr;
        
    if (parent)
        parent->addAndMakeVisible (comp);
    
    for (auto childSpec : spec->children)
        if (childSpec != nullptr)
            buildComponent (childSpec, instance, comp);

    return comp;
}

Component* UIBuilder::buildComponentInstance (const ComponentSpec* spec, UIInstance* instance, Component* parent)
{
    if (spec == nullptr)
        return nullptr;
    
    // Only the top-level composite doesn't require a parent
    //jassert (parent != nullptr || spec->type == UIComponentClass::Type::Composite);
    
    /**
     @todo:
     Yes, this isn't Object-Oriented, but creating a class tree of specs around these seems overkill?
     Proper solution: Visitor pattern makes Spec call back here with the component-specific
     configuration method directly!
     */
    switch (spec->type)
    {
        case UIComponentClass::Type::Window:
        case UIComponentClass::Type::Dialog:
            jassertfalse; // Windows are built at top level only !
            break;
            
    // Buttons
        case UIComponentClass::Type::Button:
            return new UITextButton (instance, *static_cast<const ButtonSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Toggle:
            return new UIToggleButton (instance, *static_cast<const ToggleSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Radio:
            return new UIRadioButton (instance, *static_cast<const RadioSpec*>(spec));
            break;
    // Text
        case UIComponentClass::Type::Label:
            return new UILabel (instance, *static_cast<const LabelSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Input:
            return new UITextEditor (instance, *static_cast<const InputSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Text:
            return new UITextEditor (instance, *static_cast<const TextSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Code:
            return new UICodeEditor (instance, *static_cast<const CodeSpec*>(spec));
            break;
            
    // Menus
        case UIComponentClass::Type::Combo:
            return new UIComboBox (instance, *static_cast<const ComboSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Popup:
            return new UIComboBox (instance, *static_cast<const PopupSpec*>(spec));
            break;
            
    // Lists & Tables
        case UIComponentClass::Type::List:
            return new UIListBox (instance, *static_cast<const ListSpec*>(spec));
            break;
                        
        case UIComponentClass::Type::Tree:
            return new UITreeView (instance, *static_cast<const TreeSpec*>(spec));
            break;
            
        case UIComponentClass::Type::TableHeader:
            return new UITableHeader (instance, *static_cast<const TableHeaderSpec*>(spec));
            break;
            
        case UIComponentClass::Type::TableList:
            return new UITableList (instance, *static_cast<const TableListSpec*>(spec));
            break;
            
    // Continuous Controls
        case UIComponentClass::Type::Progress:
            return new UIProgressBar(instance, *static_cast<const ProgressSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Slider:
            return new UISlider (instance, *static_cast<const SliderSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Knob:
            return new UISlider (instance, *static_cast<const KnobSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Entry:
            return new UISlider (instance, *static_cast<const EntrySpec*>(spec));
            break;
            
    // Composites
        case UIComponentClass::Type::Composite:
            return new UIComposite (instance, *static_cast<const CompositeSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Group:
            return new UIGroup (instance, *static_cast<const GroupSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Tabs:
            return new UITabComposite (instance, *static_cast<const TabsSpec*>(spec));
            break;
            
        case UIComponentClass::Type::Concertina:
            return new UIConcertina (instance, *static_cast<const ConcertinaSpec*>(spec));
            break;
            
        case UIComponentClass::Type::MenuBar:
            return new UIMenuBar (instance, *static_cast<const MenuBarSpec*>(spec));
            break;
            
        case UIComponentClass::Type::ToolBar:
            return new UIToolBar (instance, *static_cast<const ToolBarSpec*>(spec));
            break;
            
    // Graphics
        case UIComponentClass::Type::Image:
            return new UIImage (instance, *static_cast<const ImageSpec*>(spec));
            break;
            
        case UIComponentClass::Type::ImagePreview:
            return new UIImagePreview (instance, *static_cast<const ImagePreviewSpec*>(spec));
            break;
            
    // Application-specific
        case UIComponentClass::Type::Canvas:
            return new UIComposite (instance, *static_cast<const CanvasSpec*>(spec));
            break;
            
        case UIComponentClass::Type::UserDefined:
            return new UIUserDefinedComponent (instance, *static_cast<const UserDefinedSpec*>(spec));
            break;
            
        default:
            DBG ("*** ERROR: Unsupported component type specified: " << (int)(spec->type));
            break;
    }
    return nullptr;
}

bool UIBuilder::buildProxyInto (UISpec* spec, UIModel* model, UIComposite* composite, UIInstance* mockups)
{
    if (composite == nullptr)
        return false;
    
    const ComponentSpec* componentSpec = spec->getRootSpec();
    if (componentSpec == nullptr)
    {
        jassertfalse;
        return false;
    }
    
    composite->componentBuildBegin();
    auto instance = composite->getOwner()->getChildInstanceFor (model, spec);
    instance->clear();
    
    for (auto childSpec : componentSpec->children)
        buildProxy (childSpec, instance, composite, mockups);
    
    composite->componentBuildEnd();
    
    model->postBuild (*instance);
    return true;
}

Component* UIBuilder::buildProxy (const ComponentSpec* spec, UIInstance* instance, Component* parent, UIInstance* mockups)
{
    if (spec == nullptr)
        return nullptr;
    
    Component* comp = buildProxyInstance (spec, instance, parent, mockups);
    if (comp == nullptr)
        return nullptr;
    
    if (parent)
        parent->addAndMakeVisible (comp);
    
    for (auto childSpec : spec->children)
        if (childSpec != nullptr)
            buildProxy (childSpec, instance, comp, mockups);
    
    return comp;
}

Component* UIBuilder::buildProxyInstance (const ComponentSpec* spec, UIInstance* instance, Component* parent, UIInstance* mockups)
{
    if (spec == nullptr)
        return nullptr;
    
    switch (spec->type)
    {
        case UIComponentClass::Type::Window:
        case UIComponentClass::Type::Dialog:
            jassertfalse; // Windows are built at top level only !
            break;
            
        default:
            return new UIComponentProxy (instance, *spec, mockups);
            break;
    }
    return nullptr;
}

bool UIBuilder::buildEmptyCanvas (UIComposite* composite)
{
    composite->componentBuildBegin();
    composite->componentBuildEnd();
    return true;
}


void UIBuilder::buildEditLink (UIInstance* instance, UIComposite* composite)
{
    if (instance->getModel()->hasUIEditorLink())
    {
#if JUCE_DEBUG
        ScopedPointer<ButtonSpec> button1 = new ButtonSpec ("openCloneButton");
        button1->setLabel ("clone");
        button1->setLayout ((LayoutFrame("100% - 88", "4", "0", "0").withFixedWidth(40).withFixedHeight(16)));
        button1->addBinding (Bind::Action (MEMBER(&UIModel::openClone)));
        buildComponent (button1, instance, composite);
        
        ScopedPointer<ButtonSpec> button2 = new ButtonSpec ("openUIEditorButton");
        button2->setLabel ("edit");
        button2->setLayout ((LayoutFrame("100% - 44", "4", "0", "0").withFixedWidth(40).withFixedHeight(16)));
        button2->addBinding (Bind::Action (MEMBER(&UIModel::openUIEditor)));
        buildComponent (button2, instance, composite);
#endif
    }
}

//==========================================================================================================
#if 0
#pragma mark UIComponentProxy
#endif

UIComponentProxy::UIComponentProxy (UIInstance* ui, const ComponentSpec& spec, UIInstance* mockups) :
    Component (spec.identifier),
    UIAdaptor (ui, spec),
    editedSpec (const_cast<ComponentSpec*>(&spec)),
    mockupUI (mockups),
    selected (false)
{
    initialiseFromSpec (mockupUI.get(), spec);
    
    // for repaint & update
    ScopedPointer<Binding> b1 = Bind::GetValue (MEMBER(&UIEditor::getComponentSpecSelection), UIEditor::ComponentSettings);
    // for selection status update
    ScopedPointer<Binding> b2 = Bind::GetSelection (MEMBER(&UIEditor::getComponentSpecSelection), UIEditor::ComponentSelection);
    // notify model about a selection change
    ScopedPointer<Binding> b3 = Bind::Action (MEMBER(&UIEditor::updateComponentSelection));
    
    addBinding (b1->instantiateFor (getModel()));
    addBinding (b2->instantiateFor (getModel()));
    addBinding (b3->instantiateFor (getModel()));

    updateDummy();
}

UIComponentProxy::~UIComponentProxy ()
{
    deleteAllChildren();
}

void UIComponentProxy::updateDummy()
{
    dummy = nullptr; // delete & unregister
    dummy = UIBuilder::buildComponentInstance (editedSpec, mockupUI.get(), nullptr);
    dummy->setBounds (getBounds());
    repaint();
}

void UIComponentProxy::setComponentState (const Binding::Purpose& p, Selection& selection)
{
    if (p == Binding::Purpose::GetValue)
        updateDummy();
    
    if (p == Binding::Purpose::GetSelection)
    {
        if (auto uiModel = dynamic_cast<UIEditor*> (getModel()))
        {
            auto& tree = uiModel->getComponentTree();
            selected = tree.isSelected (editedSpec);
            repaint();
        }
    }
}

void UIComponentProxy::resized()
{
    if (dummy != nullptr)
    {
        dummy->setBounds (getBounds());
        dummy->resized();
    }
}

void UIComponentProxy::paint (Graphics& g)
{
    if (dummy != nullptr)
        dummy->paint (g);
    
    const int outlineThickness = selected ? 2 : 1;
    
    if (selected)
        g.fillAll (Colour(Colours::lightblue).withAlpha(0.5f));
    else
        g.fillAll (Colour(Colours::white).withAlpha(0.1f));
    
    g.setColour (Colour(Colours::white).withAlpha(0.5f));
    g.drawRect (getLocalBounds(), outlineThickness);
}

void UIComponentProxy::mouseDown (const MouseEvent& event)
{
    // Note: Selection state is managed by the tree!
    if (auto uiModel = dynamic_cast<UIEditor*> (getModel()))
    {
        auto& tree = uiModel->getComponentTree();
        if (event.mods.isCommandDown())
            tree.setSelectionOf (editedSpec, !selected);
        else
            tree.setSingleSelection (editedSpec);
    }

    performBinding (Binding::Purpose::Action);
}

void UIComponentProxy::mouseDrag (const MouseEvent& event)
{
    
}

void UIComponentProxy::mouseUp (const MouseEvent& event)
{
    
}

void UIComponentProxy::mouseDoubleClick (const MouseEvent& event)
{
    
}



} //

