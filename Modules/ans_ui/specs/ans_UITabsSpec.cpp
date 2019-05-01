/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_UITabsSpec.h"
#include "ans_UISpec.h"
#include "../core/ans_UIModel.h"

namespace ans {
    using namespace juce;


String TabPageSpec::generateSourceCPP (Model::Class* modelClass) const
{
    String out;
    out << "addPage (";
    switch (expression)
    {
        case GlobalVariable:
        {
            auto spec = getSpec.getObject()(modelClass);
            out << spec->getModelClass()->getName() << "::" << spec->getName();
            break;
        }
        case SingleLambda:
        {
            auto spec = getSpec.getObject()(modelClass);
            out << spec->getModelClass()->getName() << "::" << spec->getName() << ", "
                << "FROM_SOURCE (" << getModel.getSource() << "))";
            break;
        }
        case DualLambda:
            out << label.quoted() << ", "
                << identifier.quoted() << ", "
                << "FROM_SOURCE (" << getModel.getSource() << "), "
                << "FROM_SOURCE (" << getSpec.getSource() << ")";
            break;
    }
    out << ");";
    return out;
}


void TabPageList::addPage (const TabPageSpec& page)
{
    add (page);
}

void TabPageList::addPage (WeakReference<UISpec>& uiSpec, const TabPageSpec::GetModelExpression& modelGetter)
{
    add (TabPageSpec
         (size(),
          uiSpec->getRootSpec()->label,
          uiSpec->getRootSpec()->identifier,
          modelGetter,
          FROM_SOURCE ([&](Model::Class* m) { return uiSpec.get(); }),
          TabPageSpec::SingleLambda));
}

void TabPageList::addPage (WeakReference<UISpec>& uiSpec)
{
    add (TabPageSpec
         (size(),
          uiSpec->getRootSpec()->label,
          uiSpec->getRootSpec()->identifier,
          FROM_SOURCE ([&](UIModel* m) { return m; }),
          FROM_SOURCE ([&](Model::Class* m) { return uiSpec.get(); }),
          TabPageSpec::GlobalVariable));
}

void TabPageList::addPage (const String& theLabel,
              const ComponentID& theId,
              const TabPageSpec::GetModelExpression& modelGetter,
              const TabPageSpec::GetSpecExpression& specGetter)
{
    add (TabPageSpec
         (size(),
          theLabel,
          theId,
          modelGetter,
          specGetter,
          TabPageSpec::DualLambda));
}


// CAUTION: Must match Juce enum!
const StringArray TabsSpec::OrientationNames = {
    "TabsAtTop",
    "TabsAtBottom",
    "TabsAtLeft",
    "TabsAtRight"
};

void TabsSpec::generateSourceProperties (Model::Class* modelClass, SourceOutputStream& out) const
{
    out << identifier << "->setOrientation (TabbedButtonBar::Orientation::" << OrientationNames[orientation] << ");" << newLine;
    for (auto page : pages)
    {
        out << identifier << "->pages." << page.generateSourceCPP(modelClass) << newLine;
    }
}
    
}
