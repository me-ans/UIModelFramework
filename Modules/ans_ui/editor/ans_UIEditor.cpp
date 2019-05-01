/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_UIEditor.h"

namespace ans {
    using namespace juce;
    
UIEditor::UIEditor (UIModel::Class* client, UISpec* spec) :
    WindowUIModel(),
    modelTree (false, false),
    componentTree (true, false),
    aspectList (false, true),
    specList (false, true)
{
    // Model & spec must match or be wildcards
    jassert ((client == nullptr) == (spec == nullptr) || (spec == nullptr) ||  spec->getModelClass() == client);
    
    modelTree.setRoot (&UIModel::Class::instance());
    modelTree.setSingleSelection (client);
    
    if (modelTree.getSelection().isEmpty())
        modelTree.setSingleSelection (getUIModelClasses().getFirst());

    auto specs = getSelectedUIModelClass()->getUISpecs();
    specList.setList (specs);
    if (spec != nullptr && specs.contains (spec))
        specList.setSingleSelection (spec);
    
    inspector = new ComponentSpecInspector (this);
    updateModelSelection();    
}

UIEditor::~UIEditor ()
{    
}

Array<UISpec*> UIEditor::getUISpecs ()
{
    return getSelectedUIModelClass()->getUISpecs();
}

Array<UIModel::Class*> UIEditor::getUIModelClasses ()
{
    Array<UIModel::Class*> classes;
    UIModel::Class::instance().withAllSubClassesDo<UIModel::Class>
        ([&](auto child) { classes.add (child); });
    return classes;
}

Array<ComponentSpec::Class*> UIEditor::getComponentSpecClasses ()
{
    Array<ComponentSpec::Class*> classes;
    ComponentSpec::Class::instance().withAllSubClassesDo<ComponentSpec::Class>
        ([&](auto child) { classes.add (child); });
    return classes;
}

void UIEditor::populateLayoutCanvas (UIComposite* composite)
{
    if (auto spec = getSelectedUISpec())
        UIBuilder::buildProxyInto (spec, this, composite, composite->getOwner()->getMockupUI());
    else
        UIBuilder::buildEmptyCanvas (composite);
}


}//
