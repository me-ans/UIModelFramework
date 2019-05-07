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
    
JUCE_IMPLEMENT_SINGLETON (UIEditor)
    
    
    UIEditor::UIEditor () :
        WindowUIModel(),
        modelTree (false, false),
        componentTree (true, false),
        aspectList (false, true),
        specList (false, true)
    {
        modelTree.setRoot (&UIModel::Class::instance());
        modelTree.setSingleSelection (&UIModel::Class::instance());
        
        if (modelTree.getSelection().isEmpty())
            modelTree.setSingleSelection (getUIModelClasses().getFirst());

        inspector = new ComponentSpecInspector (this);
        updateModelSelection();
    }

    UIEditor::~UIEditor ()
    {
        clearSingletonInstance();
    }

    void UIEditor::setUISpec (UISpec* spec)
    {
        if (spec == nullptr)
            return;
        
        UIModel::Class* modelClass = dynamic_cast<UIModel::Class*> (spec->getModelClass());
        modelTree.setSingleSelection (modelClass);
        updateModelSelection();

        specList.setSingleSelection (spec);
        updateSpecSelection();
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
    
    UISpec* UIEditor::getSelectedUISpec ()
    {
        if (auto spec = specList.getSingleSelection())
            return spec;
        else
            return getSelectedUIModelClass()->getDefaultSpec();
    }

    void UIEditor::populateLayoutCanvas (UIComposite* composite)
    {
        if (auto spec = getSelectedUISpec())
            UIBuilder::buildProxyInto (spec, this, composite, composite->getUIInstance()->getMockupUI());
        else
            UIBuilder::buildEmptyCanvas (composite);
    }


}//
