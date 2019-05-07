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


/**
 @todo:
 Developer Note
 ==============
 As of Juce 5, the Component hierarchy uses raw pointers and doesn't take ownership of its children. Therefore
 unique_ptr ownership of newly created components is released here early. This must be improved by maintaining
 ownership of components in UIAdaptor!
 */

namespace ans
{
    using namespace juce;
    
std::unique_ptr<TopLevelWindow> UIBuilder::buildWindow (UISpec* spec, WindowUIModel* model)
{
    if (model == nullptr)
        return nullptr;

    const WindowSpec* windowSpec = dynamic_cast<const WindowSpec*>(spec->getRootComponentSpec());
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
    auto owner  = std::make_shared<UIInstance> (model, spec);
    auto window = std::make_unique<UIDocumentWindow> (owner, *windowSpec);
    
    if (auto contentSpec = windowSpec->children.getFirst())
        window->addComponent (buildComponent (contentSpec, owner, nullptr));

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
    
    const ComponentSpec* componentSpec = spec->getRootComponentSpec();
    if (componentSpec == nullptr)
    {
        jassertfalse;
        return false;
    }
    
    composite->componentBuildBegin();
    auto instance = composite->getUIInstance()->getChildInstanceFor (model, spec);
    instance->clear();
    
    for (auto childSpec : componentSpec->children)
        composite->addComponent (buildComponent (childSpec, instance, composite));
    
    buildEditLink (instance, composite);
    composite->componentBuildEnd();
    
    model->postBuild (*instance);
    return true;
}


std::unique_ptr<Component> UIBuilder::buildComponent (const ComponentSpec* spec, std::shared_ptr<UIInstance> instance, Component* parent)
{
    if (spec == nullptr)
        return nullptr;
    
    auto comp = spec->buildInstance (instance);
    if (comp == nullptr)
        return nullptr;
    
    if (parent)
        parent->addAndMakeVisible (comp.get());
    
    auto adaptor = dynamic_cast<UIAdaptor*> (comp.get());
    for (auto childSpec : spec->children)
    {
        if (adaptor != nullptr)
            adaptor->addComponent (buildComponent (childSpec, instance, comp.get()));
        else
            // You must not add children to components that don't derive from UIAdaptor!
            jassertfalse;
    }
    
    return comp;
}

bool UIBuilder::buildProxyInto (UISpec* spec,
                                UIModel* model,
                                UIComposite* composite,
                                std::shared_ptr<UIInstance> mockups)
{
    if (composite == nullptr)
        return false;
    
    const ComponentSpec* componentSpec = spec->getRootComponentSpec();
    if (componentSpec == nullptr)
    {
        jassertfalse;
        return false;
    }
    
    composite->componentBuildBegin();
    auto instance = composite->getUIInstance()->getChildInstanceFor (model, spec);
    instance->clear();
    
    for (auto childSpec : componentSpec->children)
        composite->addComponent (buildProxy (childSpec, instance, composite, mockups));
    
    composite->componentBuildEnd();
    model->postBuild (*instance);
    return true;
}

std::unique_ptr<Component> UIBuilder::buildProxy (const ComponentSpec* spec,
                                                  std::shared_ptr<UIInstance> instance,
                                                  Component* parent,
                                                  std::shared_ptr<UIInstance> mockups)
{
    if (spec == nullptr)
        return nullptr;
    
    auto comp = std::make_unique<UIComponentProxy> (instance, *spec, mockups);
    if (comp == nullptr)
        return nullptr;
    
    if (parent)
        parent->addAndMakeVisible (comp.get());
    
    auto adaptor = dynamic_cast<UIAdaptor*> (comp.get());
    for (auto childSpec : spec->children)
    {
        if (adaptor != nullptr)
            adaptor->addComponent (buildProxy (childSpec, instance, comp.get(), mockups));
        else
            // You must not add children to components that don't derive from UIAdaptor!
            jassertfalse;
    }

    return comp;
}

bool UIBuilder::buildEmptyCanvas (UIComposite* composite)
{
    composite->componentBuildBegin();
    composite->componentBuildEnd();
    return true;
}


void UIBuilder::buildEditLink (std::shared_ptr<UIInstance> instance, UIComposite* composite)
{
    if (instance->getModel()->hasUIEditorLink())
    {
#if JUCE_DEBUG
        std::unique_ptr<ButtonSpec> button1 = std::make_unique<ButtonSpec> ("openCloneButton");
        button1->setLabel ("clone");
        button1->setLayout ((LayoutFrame("100% - 88", "4", "0", "0").withFixedWidth(40).withFixedHeight(16)));
        button1->addBinding (Bind::Action (MEMBER(&UIModel::openClone)));
        composite->addComponent (button1->buildInstance (instance));
        
        std::unique_ptr<ButtonSpec> button2 = std::make_unique<ButtonSpec> ("openUIEditorButton");
        button2->setLabel ("edit");
        button2->setLayout ((LayoutFrame("100% - 44", "4", "0", "0").withFixedWidth(40).withFixedHeight(16)));
        button2->addBinding (Bind::Action (MEMBER(&UIModel::openUIEditor)));
        composite->addComponent (button2->buildInstance (instance));
#endif
    }
}

//==========================================================================================================
#if 0
#pragma mark UIComponentProxy
#endif

UIComponentProxy::UIComponentProxy (std::shared_ptr<UIInstance> ui,
                                    const ComponentSpec& spec,
                                    std::shared_ptr<UIInstance> mockups) :
    Component (spec.identifier),
    UIAdaptor (ui, spec),
    editedSpec (const_cast<ComponentSpec*>(&spec)),
    mockupUI (mockups),
    selected (false)
{
    initialiseFromSpec (mockupUI, spec);
    
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
}

void UIComponentProxy::updateDummy()
{
    dummy = nullptr; // need to clean all adaptors BEFORE new UI is built!
    dummy = editedSpec->buildInstance (mockupUI);
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

