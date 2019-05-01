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


INIT_SYMBOL (UIEditor, TabContents);
INIT_SYMBOL (UIEditor, TabSelection);
INIT_SYMBOL (UIEditor, ModelSelection);
INIT_SYMBOL (UIEditor, ModelSettings);
INIT_SYMBOL (UIEditor, ModelAspects);
INIT_SYMBOL (UIEditor, SpecSelection);
INIT_SYMBOL (UIEditor, ComponentSelection);
INIT_SYMBOL (UIEditor, ComponentSettings);
INIT_SYMBOL (UIEditor, ComponentLayout);
INIT_SYMBOL (UIEditor, LayoutCanvas);



WeakReference<UISpec> UIEditor::MainWindowSpec = new UISpec
(
 UIEditor::Class::instance(),
 "MainWindowSpec",
 []()
 {
    auto window = new WindowSpec ();
    window->setLayout (LayoutFrame("50% - 600", "50% - 350", "1200 relative", "700 relative"));
    window->setLabel ("UIEditor");
    window->addBinding (Bind::GetLabel (MEMBER (&UIEditor::getWindowTitle), SpecSelection));
    {
        auto content = new CompositeSpec ("content");
        {
            auto mainTabs = new TabsSpec ("mainTabs", TabContents);
            mainTabs->setOrientation (TabbedButtonBar::Orientation::TabsAtTop);
            mainTabs->pages.addPage (UIEditor::TabSpecModel);
            mainTabs->pages.addPage (UIEditor::TabSpecLayout);
            mainTabs->pages.addPage (UIEditor::TabSpecGraphics);
            mainTabs->pages.addPage (UIEditor::TabSpecResources);
            mainTabs->pages.addPage (UIEditor::TabSpecCode);
            mainTabs->addBinding (Bind::GetSelection (MEMBER (&UIEditor::getMainTabSelection), TabSelection));
            mainTabs->addBinding (Bind::SetSelection (MEMBER (&UIEditor::setMainTabSelection)));
            content->addComponent (mainTabs);
        }
        window->addComponent (content);
    }
    return window;
},
true
);




WeakReference<UISpec> UIEditor::TabSpecModel = new UISpec
(
 UIEditor::Class::instance(),
 "TabSpecModel",
 []()
 {
    auto main = new CompositeSpec ("main");
    main->setLabel ("Model");
    {
        auto l1 = new LabelSpec ("l1");
        l1->setLayout (LayoutFrame("4", "4", "25% - 2 [150..2000]", "40 relative"));
        l1->setLabel ("Classes");
        main->addComponent (l1);
    }
    {
        auto modelTree = new TreeSpec ("modelTree");
        modelTree->setLayout (LayoutFrame("4", "50", "25% - 2 [150..2000]", "100% - 150 [300..2000]"));
        modelTree->addBinding (Bind::GetValue (MEMBER (&UIEditor::getModelTree), Values));
        modelTree->addBinding (Bind::GetSelection (MEMBER (&UIEditor::getModelTreeSelection), ModelSelection));
        modelTree->addBinding (Bind::SetSelection (MEMBER (&UIEditor::setModelTreeSelection)));
        main->addComponent (modelTree);
    }
    {
        auto l2 = new LabelSpec ("l2");
        l2->setLayout (LayoutFrame("25% + 2 [154..2000]", "4", "50% - 2 [300..2000]", "40 relative"));
        l2->setLabel ("Specs");
        main->addComponent (l2);
    }
    {
        auto specsList = new ListSpec ("specsList");
        specsList->setLayout (LayoutFrame("25% + 2 [154..2000]", "50", "50% - 2 [300..2000]", "100% - 150 [300..2000]"));
        specsList->addBinding (Bind::GetValue (MEMBER (&UIEditor::getSpecsList), ModelSelection));
        specsList->addBinding (Bind::GetSelection (MEMBER (&UIEditor::getSpecsListSelection), SpecSelection));
        specsList->addBinding (Bind::SetSelection (MEMBER (&UIEditor::setSpecsListSelection)));
        main->addComponent (specsList);
    }
    {
        auto l3 = new LabelSpec ("l3");
        l3->setLayout (LayoutFrame("50% + 2 [304..2000]", "4", "50% + 200 [500..2000]", "40 relative"));
        l3->setLabel ("Aspects");
        main->addComponent (l3);
    }
    {
        auto aspectsEditor = new CompositeSpec ("aspectsEditor");
        aspectsEditor->setLayout (LayoutFrame("50% + 2 [304..2000]", "50", "50% + 200 [500..2000]", "100% - 150 [300..2000]"));
        {
            auto aspectsList = new ListSpec ("aspectsList", ModelSettings);
            aspectsList->setLayout (LayoutFrame("0", "0", "100%", "100% - 60"));
            aspectsList->addBinding (Bind::GetValue (MEMBER (&UIEditor::getAspectsList), ModelAspects));
            aspectsList->addBinding (Bind::GetSelection (MEMBER (&UIEditor::getAspectsListSelection), ModelSettings));
            aspectsList->addBinding (Bind::SetSelection (MEMBER (&UIEditor::setAspectsListSelection)));
            aspectsEditor->addComponent (aspectsList);
        }
        {
            auto aspectNew = new ButtonSpec ("aspectNew");
            aspectNew->setLayout (LayoutFrame("0", "100% - 50", "50% - 2", "100% - 28"));
            aspectNew->setLabel ("+");
            aspectNew->addBinding (Bind::Action (MEMBER (&UIEditor::newAspectName)));
            aspectNew->addBinding (Bind::GetEnabled (MEMBER (&UIEditor::getEditingEnabled), Visibility));
            aspectsEditor->addComponent (aspectNew);
        }
        {
            auto aspectRemove = new ButtonSpec ("aspectRemove");
            aspectRemove->setLayout (LayoutFrame("50% + 2", "100% - 50", "100%", "100% - 28"));
            aspectRemove->setLabel ("-");
            aspectRemove->addBinding (Bind::Action (MEMBER (&UIEditor::removeAspectName)));
            aspectRemove->addBinding (Bind::GetEnabled (MEMBER (&UIEditor::getEditingEnabled), Visibility));
            aspectsEditor->addComponent (aspectRemove);
        }
        {
            auto aspectInput = new InputSpec ("aspectInput", ModelSettings);
            aspectInput->setLayout (LayoutFrame("0", "100% - 26", "100%", "100%"));
            aspectInput->addBinding (Bind::GetValue (MEMBER (&UIEditor::getAspectName), ModelSettings));
            aspectInput->addBinding (Bind::SetValue (MEMBER (&UIEditor::setAspectName)));
            aspectInput->addBinding (Bind::GetEnabled (MEMBER (&UIEditor::getEditingEnabled), ModelSettings));
            aspectsEditor->addComponent (aspectInput);
        }
        main->addComponent (aspectsEditor);
    }
    return main;
}
);




WeakReference<UISpec> UIEditor::TabSpecLayout = new UISpec
(
 UIEditor::Class::instance(),
 "TabSpecLayout",
 []()
 {
    auto tabLayout = new CompositeSpec ("tabLayout");
    tabLayout->setLabel ("Layout");
    {
        auto leftPart = new CompositeSpec ("leftPart");
        leftPart->setLayout (LayoutFrame("4", "4", "25% - 2 [150..300]", "100% - 4"));
        {
            auto componentTree = new TreeSpec ("componentTree");
            componentTree->setMultipleSelectionEnabled (true);
            componentTree->addBinding (Bind::GetValue (MEMBER (&UIEditor::getComponentTree), SpecSelection));
            componentTree->addBinding (Bind::GetSelection (MEMBER (&UIEditor::getComponentSpecSelection), ComponentSelection));
            componentTree->addBinding (Bind::SetSelection (MEMBER (&UIEditor::setComponentSpecSelection)));
            leftPart->addComponent (componentTree);
        }
        tabLayout->addComponent (leftPart);
    }
    {
        auto middlePart = new CompositeSpec ("middlePart");
        middlePart->setLayout (LayoutFrame("25% + 2 [150..300]", "4", "100% - 250", "100% - 4"));
        middlePart->setBackgroundColour (SOURCE (Colours::darkkhaki));
        {
            auto layoutCanvas = new CanvasSpec ("layoutCanvas");
            layoutCanvas->addBinding (Bind::Canvas (MEMBER (&UIEditor::populateLayoutCanvas), LayoutCanvas));
            middlePart->addComponent (layoutCanvas);
        }
        tabLayout->addComponent (middlePart);
    }
    {
        auto rightPart = new CompositeSpec ("rightPart");
        rightPart->setLayout (LayoutFrame("100% - 246", "4", "100% - 4", "100% - 4"));
        {
            auto inspectorCanvas = new CanvasSpec ("inspectorCanvas");
            inspectorCanvas->addBinding (Bind::Canvas (MEMBER (&UIEditor::populateInspectorCanvas), ComponentSelection));
            rightPart->addComponent (inspectorCanvas);
        }
        tabLayout->addComponent (rightPart);
    }
    return tabLayout;
}
);




WeakReference<UISpec> UIEditor::TabSpecGraphics = new UISpec
(
 UIEditor::Class::instance(),
 "TabSpecGraphics",
 []()
 {
    auto tabGraphics = new CompositeSpec ("tabGraphics");
    tabGraphics->setLabel ("Graphics");
    {
        auto l1 = new LabelSpec ("l1");
        l1->setLayout (LayoutFrame("4", "4", "100% - 4", "40 relative"));
        l1->setLabel ("...");
        tabGraphics->addComponent (l1);
    }
    return tabGraphics;
}
);




WeakReference<UISpec> UIEditor::TabSpecResources = new UISpec
(
 UIEditor::Class::instance(),
 "TabSpecResources",
 []()
 {
    auto tabResources = new CompositeSpec ("tabResources");
    tabResources->setLabel ("Resources");
    {
        auto l1 = new LabelSpec ("l1");
        l1->setLayout (LayoutFrame("4", "4", "100% - 4", "40 relative"));
        l1->setLabel ("...");
        tabResources->addComponent (l1);
    }
    return tabResources;
}
);




WeakReference<UISpec> UIEditor::TabSpecCode = new UISpec
(
 UIEditor::Class::instance(),
 "TabSpecCode",
 []()
 {
    auto tabCode = new CompositeSpec ("tabCode");
    tabCode->setLabel ("Code");
    {
        auto label1 = new LabelSpec ("label1");
        label1->setLayout (LayoutFrame("4", "4", "100% - 210", "22 relative"));
        label1->addBinding (Bind::GetLabel (MEMBER (&UIEditor::getCodeLabel), Labels));
        tabCode->addComponent (label1);
    }
    {
        auto saveButton = new ButtonSpec ("saveButton");
        saveButton->setLayout (LayoutFrame("100% - 204", "4", "200 relative", "22 relative"));
        saveButton->setLabel ("Save Now");
        saveButton->addBinding (Bind::GetEnabled (MEMBER (&UIEditor::getEditingEnabled), Visibility));
        saveButton->addBinding (Bind::Action (MEMBER (&UIEditor::saveButtonClicked)));
        tabCode->addComponent (saveButton);
    }
    {
        auto codeHPP = new CodeSpec ("codeHPP");
        codeHPP->setLayout (LayoutFrame("4", "28", "100% - 4", "50% - 2"));
        codeHPP->addBinding (Bind::GetValue (MEMBER (&UIEditor::getCodeHPP), ModelSettings));
        tabCode->addComponent (codeHPP);
    }
    {
        auto codeCPP = new CodeSpec ("codeCPP");
        codeCPP->setLayout (LayoutFrame("4", "50% + 2", "100% - 4", "100% - 4"));
        codeCPP->addBinding (Bind::GetValue (MEMBER (&UIEditor::getCodeCPP), ModelSettings));
        tabCode->addComponent (codeCPP);
    }
    return tabCode;
}
);




} // namespace


