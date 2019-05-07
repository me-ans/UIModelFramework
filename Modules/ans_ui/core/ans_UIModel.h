/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_BindingTemplates.h"
#include "../adaptors/ans_adaptor_base.h"
#include "../specs/ans_UISpec.h"
#include "../utility/ans_LayoutCursor.h"

namespace ans {
    using namespace juce;


class UIBuilder;
class UIComposite;

/**
 Any arbitrary part of your application that interacts with a GUI can derive from UIModel.
 Think of it as "application code linked with a user interface". From an MVC perspective,
 it implements a model. UIModel is an abstract base class that must not be instantiated.
 Use WindowUIModel, or EmbeddedUIModel if it's nested into another UIModel.
 */

class UIModel : public Model
{
public:
    
    /** Symbols used for aspects of UIModel */
    
    DEFINE_SYMBOL (Configs);        ///< (Re)configures all components with a Config binding by asking UIModel to do so
    DEFINE_SYMBOL (Canvases);       ///< (Re)populates all UIComposites with a Canvas binding by asking UIModel to do so
    DEFINE_SYMBOL (Visibility);     ///< Updates visibility of all components with a GetVisibility binding
    DEFINE_SYMBOL (Enablement);     ///< Updates enablement of all components with a GetEnabled binding
    DEFINE_SYMBOL (Labels);         ///< Updates the labels of all components with a GetLabel binding
    DEFINE_SYMBOL (Values);         ///< Updates the main contents of all components with a GetValue binding
    DEFINE_SYMBOL (Selections);     ///< Updates the selection state of all components with a GetSelection binding
    DEFINE_SYMBOL (WindowLayout);   ///< Closes all windows currently open on the top-level WindowUIModel
    DEFINE_SYMBOL (WindowClose);    ///< Re-deploys layouts from UISpec that has been edited
    
    
    METACLASS_BEGIN_DERIVED (UIModel, Model, UIItemInterface)

    Array<Aspect> getAspects() const override
    {
        return { Configs, Canvases, Visibility, Enablement, Labels, Values, Selections, WindowLayout, WindowClose };
    }
    
    /**
     Changes aspect names temporarily only, on behalf of UIEditor, for subsequent source
     code generation. This has no effect on a running program!
     */
    void setAspectNames (const Array<String>& editedNames);
    
    /**
     Return the default UISpec to use if no specific spec was provided. By default this is
     any of the model's specs which is tagged as default. You may override this, if you want
     to prevent UIEditor from changing your choosen default.
     */
    virtual UISpec* getDefaultSpec();
    
    /** Generate C++ source code for the entire *.specs.cpp file that accompanies this UIModel */
    String generateSourceCPP ();
    
    /** Generate C++ source code for the entire *.specs.h file that accompanies this UIModel */
    String generateSourceHPP ();
    
    /** Generate C++ source code for all *.specs.* files that accompany this UIModel */
    void generateSourceFiles ();
    
    /** Generate C++ source code for all *.specs.* files that accompany this UIModel */
    void generateSourceFiles (const File& targetFolder);
    
    /**
     Register a UISpec in an array of weak references (once a spec is deleted, it will
     be dropped from that array automatically)
     */
    virtual void addSpec (UISpec* spec);
    
    /** Get all currently known specs of this UIModel */
    virtual Array<UISpec*> getUISpecs();
    
    bool isEditable() override { return false; }
    
    /**
     Every UIModel knows the C++ filename of its associated *.specs.h, so a UI Editor can save an edited
     UISpec to that location. User-defined and editable UIModels must override this in their header.
     */
    virtual String getSpecsFilename() const { return __FILE__; }
    
    /** Derive the actual File from an often relative getSpecsFilename() */
    File getSpecsFile() const;
    
    /** Get the contents of a source code template file to be used for code generation */
    const String getTemplate (const String& name);
    
    // UIItemInterface methods:
    
    const String getItemString() const override { return getName(); }
    
    const ComponentID getItemIdentifier() override { return getName(); }
    
    bool itemHasChildren() override { return ! getSubClasses().isEmpty(); }
    
    Array<UIItemInterface*> getItemChildren() override { return UIItemInterface::convert (getSubClasses()); }
    
    
    private:
    StringArray generateAspectNames();
    
    WeakReference<UISpecRegistry> specs;
    Array<String> changedAspectNames;
    
    METACLASS_END

    //=======================================================================================
    
    static WeakReference<UISpec> DefaultSpec;
    
    
    UIModel();
    virtual ~UIModel();
    
    /** Return yourself */
    UIModel* yourself() { return this; }
    
    /** Whether the model is a WindowUIModel */
    virtual bool isWindowModel() { return false; }
    
    /** Return the parent model of the receiver */
    virtual UIModel* getParentModel() { return nullptr; }
    
    /** Return the top-most UIModel in the hierarchy. This is a WindowUIModel in most cases */
    virtual UIModel* topLevelModel() { return this; }
    
    /** Unique identifier of a model inside the scope of its top-level WindowUIModel */
    Identifier getUniqueID ();
    
    /**
     Populate a UIComposite with components created from a UISpec and bind those to this model.
     The parent component may be part of another UIModel's user interface.
     */
    bool populateComposite (UIComposite* composite, UISpec* uiSpec);
    bool populateComposite (UIComposite* composite);
    
    /**
     Called by UIBuilder after building a UISpec. Subclasses can hook in here
     to do some setup work before the UI goes live. If so, they must also call
     Parent::postBuild (instance) to ensure proper setup.
     */
    virtual void postBuild (UIInstance& instance);
    
    //=====================================================================================
    
    void registerEmbeddedModel (UIModel* child) { embeddedModels.addIfNotAlreadyThere (child); }
    void removeEmbeddedModel (UIModel* child) { embeddedModels.removeFirstMatchingValue (child); }
    
    /** Open a UIEditor on the model (development buils only) */
    virtual void openUIEditor();
    
    virtual bool hasUIEditorLink() { return getClass()->isEditable(); }
    
    /** Open a new window on the same model (development builds only) */
    virtual void openClone();
    
    
friend class UIAdaptor;
friend class UIBuilder;
protected:
    
    Identifier identifier;
    Array<WeakReference<UIModel>> embeddedModels; // for enumeration and reference
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (UIModel)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIModel)
};


/**
 EmbeddedUIModel is the abstract base class of user interface models that are nested into others.
 (on these a window can't be opened). For example, the individual pages of a tabbed component could be
 built and managed by a EmbeddedUIModel each, in effect separating the application code of each
 tab from each other.
 */

class EmbeddedUIModel :
        public UIModel
{
public:
    
    METACLASS_BEGIN (EmbeddedUIModel, UIModel)
    
    Array<Aspect> getAspects() const override { return {}; }
    
    bool isEditable() override { return this != &EmbeddedUIModel::Class::instance(); }
    
    const String getNameSpace() override
    {
        return (this == &EmbeddedUIModel::Class::instance())
            ? Super::getNameSpace()
            : "";
    }
    
    METACLASS_END

    
    EmbeddedUIModel (UIModel* parent) : parentModel(parent)
    {
        parent->registerEmbeddedModel(this);
    }
    
    virtual ~EmbeddedUIModel()
    {
        if (parentModel != nullptr)
            parentModel->removeEmbeddedModel (this);
    }
    
    UIModel* getParentModel() override { return parentModel; }
    
    UIModel* topLevelModel() override { return parentModel->topLevelModel(); }

private:
    WeakReference<UIModel> parentModel;
};

/**
 WindowUIModel is the abstract base class of user interface models on which a window can be opened.
 */

class WindowUIModel : public UIModel
{
public:
    
    METACLASS_BEGIN (WindowUIModel, UIModel)

    Array<Aspect> getAspects() const override { return {}; }
    
    bool isEditable() override { return this != &WindowUIModel::Class::instance(); }
    
    const String getNameSpace() override
    {
        return (this == &WindowUIModel::Class::instance())
            ? Super::Class::getNameSpace()
            : "";
    }
    
    METACLASS_END
    
    
    WindowUIModel (WindowUIModel * master = nullptr);
   ~WindowUIModel ();
    
    bool isWindowModel() override { return true; }
    
    /** Subclasses may override this to provide a dynamic window title */
    virtual const String getWindowTitle () { return "Untitled"; }
    
    /** Called only before this model is opened by the App on launch */
    virtual void initialise (const String& commandLine);
    
    /**
     Creates a new window for this model, populated with the supplied UISpec.
     Be sure to take ownership of the window pointer, or use std::release() to let
     TopLevelWindowManager take control. Otherwise it will close immediately after going out of scope.
     */
    std::unique_ptr<TopLevelWindow> createWindow (UISpec* spec, bool open = true, const Rectangle<int>* frame = nullptr);
    
    /** Open a window on the default UISpec and let TopLevelWindowManager take control of it */
    void open ();
    
    /** Open a window on the supplied UISpec and let TopLevelWindowManager take control of it */
    void open (UISpec* spec);
    
    /** Open a window on the supplied UISpec in a specific desktop frame and let TopLevelWindowManager take control of it */
    void open (UISpec* spec, const Rectangle<int>& frame);
    
    /** Programmatically close all windows currently open on this model */
    void closeAllWindows();
    
    /** Whether a currently open window may be closed */
    virtual bool canCloseWindow() { return true; }
    
    /**
     Called after opening a window on a new instance. Subclasses can hook in here
     to do some setup work after the UI went live. If so, they must also call
     Parent::postBuild (instance) to ensure proper setup.
     */
    virtual void postOpen (UIInstance& instance);

    /**
     Called after a window currently open on the model was closed and deleted. The UIInstance
     will be deleted after returning from this method. Derived classes may override this, but
     must call Parent::postClose() to ensure proper cleanup.
     */
    virtual void postClose (UIInstance& instance);
    
    /** Open a new window on the same model (development builds only) */
    void openClone() override;
    
    bool hasOpenWindows();
    
private:
    WeakReference<UIModel> masterWindowModel;
    
};
    
}
