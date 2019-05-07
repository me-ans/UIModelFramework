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

/** Represents a single tab page and where to get its contents */

struct TabPageSpec
{
    // @todo: Make this in the Binding style w/base & templated derived
    using GetModelExpression = ObjectAndSource<std::function<UIModel*(UIModel*)>>;
    using GetSpecExpression  = ObjectAndSource<std::function<UISpec*(Model::Class*)>>;
    
    typedef enum { GlobalVariable, SingleLambda, DualLambda } Expression;
    
    TabPageSpec (int theIndex,
                 const String& theLabel,
                 const ComponentID& theId,
                 GetModelExpression modelGetter,
                 GetSpecExpression specGetter,
                 Expression e = DualLambda) :
        index (theIndex),
        label (theLabel),
        identifier (theId),
        getModel (modelGetter),
        getSpec (specGetter),
        expression (e)
    {}
    
    virtual ~TabPageSpec() {}
    
    String generateSourceCPP (Model::Class* modelClass) const;
    
    int index;
    String label;
    ComponentID identifier;
    GetModelExpression getModel;
    GetSpecExpression getSpec;
    Expression expression;
};

/**
 TabPageList is used by UIModel to supply a tabbed composite with pages.
 */

struct TabPageList : public Array<TabPageSpec>
{
    using Shared = std::shared_ptr<TabPageList>;
    
    void addPage (const TabPageSpec& page);
    void addPage (WeakReference<UISpec>& uiSpec, const TabPageSpec::GetModelExpression& modelGetter);
    void addPage (WeakReference<UISpec>& uiSpec);
    void addPage (const String& theLabel,
                  const ComponentID& theId,
                  const TabPageSpec::GetModelExpression& modelGetter,
                  const TabPageSpec::GetSpecExpression& specGetter);
    
    JUCE_LEAK_DETECTOR (TabPageList)
};


/**
 TabsSpec builds a composite of a UITabbedComposite and replaceable content components,
 each of which refers to a UISpec and optionally to a separate UIModel.
 */
struct TabsSpec : public CompositeSpecBase
{
    /** Labels used for generating code and parsing input */
    static const StringArray OrientationNames;
    
    METACLASS_BEGIN (TabsSpec, CompositeSpecBase)
    METACLASS_END
    
    TabsSpec (const ComponentID& n, Aspect a = Model::Undefined) :
        CompositeSpecBase (UIComponentClass::Type::Tabs, n, a)
    {}
    
    std::unique_ptr<Component> buildInstance (std::shared_ptr<UIInstance> instance) const override;
    
    void setOrientation (TabbedButtonBar::Orientation o) { orientation = o; }
    
    void generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const override;
    
    TabbedButtonBar::Orientation orientation = TabbedButtonBar::Orientation::TabsAtTop;
    TabPageList pages;
};

}
