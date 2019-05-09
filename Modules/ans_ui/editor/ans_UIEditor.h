/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "inspector/ans_ComponentSpecInspector.h"

namespace ans {
    using namespace juce;

    /**
     UIEditor is a global singleton for editing UISpec and UIModel classes at runtime.
     
     UIEditor::getInstance()->setSpec (aUISpec);
     UIEditor::getInstance()->open();
     */
    
    class UIEditor  :
            public WindowUIModel,
            public DeletedAtShutdown
    {
    public:
        
    #include "ans_UIEditor.specs.h"
        
        JUCE_DECLARE_SINGLETON (UIEditor, false)
        
        UIEditor ();
       ~UIEditor ();
        
        using ModelTree     = UITreeModel<UIModel::Class>;
        using SpecList      = UIListModel<UISpec>;
        using AspectList    = UIListModel<String>;
        using ComponentTree = UITreeModel<ComponentSpec>;
        
        /** Example: UIEditor::getInstance()->setSpec (spec) */
        void setUISpec (UISpec* specToEdit);
        
        const String getWindowTitle () override
        {
            auto spec = getSelectedUISpec();
            if (auto mc = getSelectedUIModelClass())
                return mc->getName() + "::" + (spec == nullptr ? "*" : spec->getName());
            else
                return "UIEditor";
        }
        
        // Global Tabs

        int  getMainTabSelection () { return selectedMainTabIndex; }
        void setMainTabSelection (int value) { selectedMainTabIndex = value; }
        
        const String getSpecName ()  { return getSelectedUISpec()->getName(); }
        const String getModelName () { return getSelectedUIModelClass()->getName(); }
        bool  getEditingEnabled ()   { return getSelectedUIModelClass()->isEditable(); }
        
        // Model Tree
        
        ModelTree& getModelTree ()           { return modelTree; }
        Selection& getModelTreeSelection ()  { return modelTree.getSelection(); }
        void       setModelTreeSelection (Selection& selection)
        {
            modelTree.setSelection (selection);
            updateModelSelection();
        }

        // Aspect List & Editor
        
        AspectList& getAspectsList ()           { return aspectList; }
        Selection&  getAspectsListSelection ()  { return aspectList.getSelection(); }
        void        setAspectsListSelection (Selection& selection)
        {
            aspectList.setSelection (selection);
            changed (ModelSettings);
        }

        String getAspectName ()
        {
            if (auto name = aspectList.getSingleSelection())
                return *name;
            else
                return "";
        }
        
        void setAspectName (const String& name)
        {
            int index = aspectList.getSelectedIndex();
            if (index >= 0)
            {
                aspectListEditBuffer.set (aspectList.getSelectedIndex(), name);
                getSelectedUIModelClass()->setAspectNames (aspectListEditBuffer);
                changed (ModelSettings);
            }
        }
        
        void newAspectName ()
        {
            aspectListEditBuffer.insert (aspectList.getSelectedIndex(), "NewAspect");
            aspectList.setListReference (aspectListEditBuffer);
            getSelectedUIModelClass()->setAspectNames (aspectListEditBuffer);
            changed ({ModelAspects, ModelSettings});
        }
        
        void removeAspectName ()
        {
            int index = aspectList.getSelectedIndex();
            if (index >= 0)
            {
                aspectListEditBuffer.remove (index);
                getSelectedUIModelClass()->setAspectNames (aspectListEditBuffer);
                aspectList.setListReference (aspectListEditBuffer);
                aspectList.setSelectedIndex (jmax (0, index - 1));
                changed ({ModelAspects, ModelSettings});
            }
        }

        // UISpecs
        
        SpecList&  getSpecsList ()         { return specList; }
        Selection& getSpecsListSelection() { return specList.getSelection(); }
        void       setSpecsListSelection (Selection& selection)
        {
            specList.setSelection (selection);
            
            if (specList.getSelection().isEmpty())
                specList.setSingleSelection (getSelectedUIModelClass()->getDefaultSpec());
            
            updateSpecSelection();
        }
        
        // Component Tree
        
        ComponentTree& getComponentTree ()         { return componentTree; }
        Selection&     getComponentSpecSelection() { return componentTree.getSelection(); };
        void           setComponentSpecSelection (Selection& selection)
        {
            componentTree.setSelection (selection);
            updateComponentSelection();
        }
        
        // Visual Layout Editing
        void populateLayoutCanvas (UIComposite* canvas);
        
        // Component Inspector
        void populateInspectorCanvas (UIComposite* canvas) { inspector->populateComposite (canvas); }

        // Code
        const String getCodeLabel() { return getSelectedUIModelClass()->getSpecsFile().getFullPathName(); }
        const String getCodeCPP()   { return getSelectedUIModelClass()->generateSourceCPP(); }
        const String getCodeHPP()   { return getSelectedUIModelClass()->generateSourceHPP(); }
        void saveButtonClicked()    { getSelectedUIModelClass()->generateSourceFiles(); }

        
        //  Updating
        
        /** Propagate a change of selected UIModel class */
        void updateModelSelection()
        {
            if (getSelectedUIModelClass() == nullptr)
                modelTree.setSingleSelection (getUIModelClasses().getFirst());
            
            aspectListEditBuffer = getSelectedUIModelClass()->getAspectNames();
            aspectList.setListReference (aspectListEditBuffer);
            
            specList.setList (getSelectedUIModelClass()->getUISpecs());
            specList.setSelectedIndex (0);
            
            changed ({ModelSelection, ModelAspects, ModelSettings, Visibility});
            updateSpecSelection();
        }
        
        /** Propagate a change of selected UISpec */
        void updateSpecSelection()
        {
            if (getSelectedUISpec() != nullptr)
                componentTree.setRoot (getSelectedUISpec()->getRootComponentSpec());
            else
                componentTree.setRoot (nullptr);
            
            changed ({SpecSelection, LayoutCanvas});
            updateComponentSelection();
        }
        
        /** Propagate a change of selected ComponentSpec */
        void updateComponentSelection()
        {
            inspector->setSelectedComponentSpec (componentTree.getSingleSelection());
            changed ({ComponentSelection, ComponentSettings});
        }
        
        void updateLayoutCanvas()
        {
            changed (LayoutCanvas);
        }
        
        //  Opening/Closing
        
        void postBuild (UIInstance& instance) override
        {
            if (instance.isForWindow())
                changed ({TabContents, ModelSelection});
            
            Parent::postBuild (instance);
        }
        
        void postClose (UIInstance& instance) override
        {
            Parent::postClose (instance);
        }
        
        bool hasUIEditorLink() override { return false; }
        
        //  Static & Global
        
        /** Get all UISpec of the selected UIModel::Class */
        Array<UISpec*> getUISpecs ();
        
        /** Get all subclasses of UIModel that are globally reachable */
        Array<UIModel::Class*> getUIModelClasses ();
        
        /** Get all subclasses of ComponentSpec incl. itself */
        Array<ComponentSpec::Class*> getComponentSpecClasses ();
        
        //--- Selection & Focus ------------------------------------------------------------
        
        /** Get the UISpec being edited */
        UISpec* getSelectedUISpec();
        
        /** Get the UIModel being edited */
        UIModel::Class* getSelectedUIModelClass () { return modelTree.getSingleSelection(); }
        
        const ComponentID getSuggestedID (const ComponentID& base)
        {
            return getSelectedUISpec()->getUniqueComponentID (base);
        }
        
    private:
        int             selectedMainTabIndex = 1;
        ModelTree       modelTree;
        ComponentTree   componentTree;
        Array<String>   aspectListEditBuffer;   // physical strings for editing
        AspectList      aspectList;             // pointers to the above
        SpecList        specList;
        
        ScopedPointer<ComponentSpecInspector> inspector;
    };
    
}
