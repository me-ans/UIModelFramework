/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_tabs.h"

#include "../core/ans_UIModel.h"
#include "../utility/ans_Positioners.h"

namespace ans {
    using namespace juce;


UITabComposite::UITabComposite (UIInstance* owner, const TabsSpec& spec) :
    UIComposite (owner, spec),
    inhibit (false),
    currentPopulatedIndex (-1),
    tabBar (new TabbedButtonBar (spec.orientation)),
    tabContents (new UIComposite (owner, spec.identifier + "_Content"))
{
    int h = 30;
    {
        LayoutFrame frame = LayoutFrame::entire();
        switch (spec.orientation)
        {
            case TabbedButtonBar::Orientation::TabsAtLeft:   frame.right = {0.0,h}; break;
            case TabbedButtonBar::Orientation::TabsAtTop:    frame.bottom = {0.0,h}; break;
            case TabbedButtonBar::Orientation::TabsAtRight:  frame.left = {1.0,-h}; break;
            case TabbedButtonBar::Orientation::TabsAtBottom: frame.top = {1.0,-h}; break;
        }
        addComponent (tabBar, frame);
        tabBar->addChangeListener (this);
    }
    {
        LayoutFrame frame = LayoutFrame::entire();
        switch (spec.orientation)
        {
            case TabbedButtonBar::Orientation::TabsAtLeft:   frame.left.offset += h; break;
            case TabbedButtonBar::Orientation::TabsAtTop:    frame.top.offset += h; break;
            case TabbedButtonBar::Orientation::TabsAtRight:  frame.right.offset -= h; break;
            case TabbedButtonBar::Orientation::TabsAtBottom: frame.bottom.offset -= h; break;
        }
        addComponent (tabContents, frame);
    }
    
    if (!spec.pages.isEmpty())
    {
        pages = spec.pages;
        buildPages();
    }
    inhibit = false;
}


UITabComposite::~UITabComposite ()
{
    if (tabBar != nullptr)
        tabBar->removeChangeListener (this);
    
    notificationPre = nullptr;
    notificationPost = nullptr;
    confirmation = nullptr;
}

void UITabComposite::deleteContents ()
{
    tabBar = nullptr;
    tabContents = nullptr;
    UIComposite::deleteContents();
}

int UITabComposite::getSelectedIndex ()
{
    return tabBar->getCurrentTabIndex();
}

void UITabComposite::setSelectedIndex (int index)
{
    tabBar->setCurrentTabIndex (index, false);
    
    if (index != currentPopulatedIndex)
    {
        currentPopulatedIndex = index;
        populateContents (index);
    }
}

ComponentID UITabComposite::getSelectedID ()
{
    int i = getSelectedIndex();
    if (i < 0 || i > pages.size())
        return "";
    else
        return pages.getReference(i).identifier;
}

void UITabComposite::clearPages ()
{
    pages.clear();
}

void UITabComposite::buildPages ()
{
    tabBar->clearTabs();
    for (auto page : pages)
        tabBar->addTab (page.label, Colours::black, -1);
}

void UITabComposite::addPage (const String& label,
                              const ComponentID& pageId,
                              TabPageSpec::GetModelExpression modelGetter,
                              TabPageSpec::GetSpecExpression specGetter)
{
    pages.addPage (label, pageId, modelGetter, specGetter);
}

void UITabComposite::populateContents (int index)
{
    if (index < 0 || index >= pages.size())
        return;
    
    const TabPageSpec& page = pages.getReference(index);
    
    if (notificationPre != nullptr)
        notificationPre();
    
    if (page.getModel.getObject() == nullptr)
    {
        // Local UIModel
        UISpec* spec = UIAdaptor::getModel()->getClass()->getDefaultSpec();
        if (page.getSpec.getObject() != nullptr)
            spec = page.getSpec.getObject()(getModel()->getClass());
        
        UIAdaptor::getModel()->populateComposite (tabContents, spec);
        
    } else {
        // External UIModel or EmbeddedUIModel
        UIModel* externalModel = page.getModel.getObject()(getModel());
        if (externalModel == nullptr)
            return;
        
        UISpec* spec = externalModel->getClass()->getDefaultSpec();
        if (page.getSpec.getObject() != nullptr)
            spec = page.getSpec.getObject()(getModel()->getClass());
        
        externalModel->populateComposite (tabContents, spec);
    }
    
    if (notificationPost != nullptr)
        notificationPost();
}


void UITabComposite::changeListenerCallback (ChangeBroadcaster* source)
{
    if (getSelectedIndex() != currentPopulatedIndex)
    {
        currentPopulatedIndex = getSelectedIndex();
        performBinding (Binding::Purpose::SetSelection);
        populateContents (currentPopulatedIndex);
    }
}

void UITabComposite::getComponentState (const Binding::Purpose& p, var& value)
{
    if (p == Binding::Purpose::SetSelection)
    {
        value = currentPopulatedIndex;
        return;
    }
    
    if (p == Binding::Purpose::SetLabel)
    {
        value = pages.getReference (currentPopulatedIndex).label;
        return;
    }
    UIAdaptor::getComponentState (p, value);
}

void UITabComposite::setComponentState (const Binding::Purpose& p, const var& value)
{
    if (p == Binding::Purpose::GetSelection)
        return setSelectedIndex (value);

    if (p == Binding::Purpose::GetValue)
    {
        if (TabPageList::Ptr tabs = dynamic_cast<TabPageList*>(value.getObject()))
        {
            clearPages();
            for (auto page : *tabs)
                pages.addPage (page);
            buildPages();
            return;
        }
    }
    
    if (p == Binding::Purpose::GetLabel)
    {
        pages.getReference (currentPopulatedIndex).label = value.toString();
        tabBar->setTabName (currentPopulatedIndex, pages.getReference (currentPopulatedIndex).label);
        return;
    }
    UIAdaptor::setComponentState (p, value);
}

}
