/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */


#pragma once

#include "ans_ComponentSpec.h"

namespace ans {
    using namespace juce;

class UIModel;

/**
 UISpec is at the centre of the UI framework. It is a native definition of a UI. Any UIModel owns one
 or more UISpec, e.g. for each page of a tabbed component, or for different views on the same
 data (e.g. a beginner's UI vs. a Pro UI). Instances of UISpec are global and owned by UIModel::Class
 and subclasses.
 */

class UISpec : public UIItemInterface
{
public:
    
    typedef std::function<ComponentSpec*()> SpecLambda;
    
    /** Constructor for global UISpec, as used in generated *.specs.cpp files */
    UISpec (Model::Class* modelClassRef, const String& specName_, SpecLambda contentCreator, bool beDefault = false);
    
    /** Constructor for temporary specs for programmatical use (takes ownership of content) */
    UISpec (Model::Class* modelClassRef, std::unique_ptr<ComponentSpec> content);
    
   ~UISpec ();
    
    const String& getName() const { return specName; }
    Model::Class* getModelClass() const { return modelClass; }
    
    ComponentSpec*  getComponentSpec (const ComponentID& identifier);
    const String  getUniqueComponentID (const ComponentID& identifier, ComponentSpec* exclude = nullptr);
    
    /**
     Return the ComponentSpec provided by this UISpec. This may be a WindowSpec or a CompositeSpec,
     depending on whether this is for a window, or for an embedded UI
     */
    ComponentSpec* getRootComponentSpec ();
    
    /** Generate C++ source code for the .specs.cpp file of the model */
    String generateSourceCPP (Model::Class* modelClass);
    
    /** Collect and return all aspects used by components in this UI */
    Aspects getUsedAspects();
    
    /** Whether this is a UIModel's default spec  */
    bool isDefault() { return defaultSpec; }

    /** Set whether this is a UIModel's default spec  */
    void setDefault (bool on) { defaultSpec = on; }
    
    /** Delete any chached ComponentSpec */
    void flush();
    
    /** Render the name for display in lists & trees */
    const String getItemString() const override { return getName(); }

private:
    std::unique_ptr<ComponentSpec> createSpec ();
    ComponentSpec* findComponentSpec (const ComponentID& identifier, ComponentSpec* current, ComponentSpec* exclude = nullptr);
    
    Model::Class* modelClass;
    String specName;
    SpecLambda specLambda;
    std::unique_ptr<ComponentSpec> rootSpec;
    File filename;
    bool defaultSpec;
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (UISpec)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UISpec)
};


/**
 UISpecRegistry is a small utility class that keeps references to UISpec on behalf of
 UIModel::Class and takes care of deleting them at shutdown.
 */

class UISpecRegistry :
        public DeletedAtShutdown
{
public:
    UISpecRegistry ();
   ~UISpecRegistry ();

    void addSpec (UISpec* spec);
    Array<UISpec*> getUISpecs();

private:
    void collectGarbage();
    
    Array<WeakReference<UISpec>> specs;
    bool shuttingDown = false;
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (UISpecRegistry)
};


}
