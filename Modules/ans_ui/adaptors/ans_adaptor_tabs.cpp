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


UITabComposite::UITabComposite (std::shared_ptr<UIInstance> instance, const TabsSpec& spec) :
    UIComposite (instance, spec),
    inhibit (false),
    currentPopulatedIndex (-1),
    tabBarRef (nullptr),
    tabContentsRef (nullptr)
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
        auto tabBarComp = std::make_unique<TabbedButtonBar> (spec.orientation);
        tabBarComp->addChangeListener (this);
        tabBarRef = tabBarComp.get();
        addComponent (std::move(tabBarComp), frame);
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
        auto tabContentsComp = std::make_unique<UIComposite> (instance, spec.identifier + "_Content");
        tabContentsRef = tabContentsComp.get();
        addComponent (std::move(tabContentsComp), frame);
    }
    
    if (!spec.pages.isEmpty())
    {
        pages = std::make_shared<TabPageList> (spec.pages);
        buildPages();
    }
    inhibit = false;
}


UITabComposite::~UITabComposite ()
{
    if (tabBarRef != nullptr)
        tabBarRef->removeChangeListener (this);
    
    notificationPre = nullptr;
    notificationPost = nullptr;
    confirmation = nullptr;
}

void UITabComposite::deleteContents ()
{
    tabBarRef = nullptr;
    tabContentsRef = nullptr;
    UIComposite::deleteContents();
}

int UITabComposite::getSelectedIndex ()
{
    return tabBarRef->getCurrentTabIndex();
}

void UITabComposite::setSelectedIndex (int index)
{
    tabBarRef->setCurrentTabIndex (index, false);
    
    if (index != currentPopulatedIndex)
    {
        currentPopulatedIndex = index;
        populateContents (index);
    }
}

ComponentID UITabComposite::getSelectedID ()
{
    int i = getSelectedIndex();
    if (i < 0 || i > getPages()->size())
        return "";
    else
        return getPages()->getReference(i).identifier;
}
    
std::shared_ptr<TabPageList> UITabComposite::getPages()
{
    if (pages == nullptr)
        pages = std::make_shared<TabPageList>();
    return pages;
}

void UITabComposite::clearPages ()
{
    if (pages != nullptr)
        pages->clear();
}

void UITabComposite::buildPages ()
{
    tabBarRef->clearTabs();
    
    if (pages != nullptr)
        for (auto page : *pages)
            tabBarRef->addTab (page.label, Colours::black, -1);
}

void UITabComposite::addPage (const String& label,
                              const ComponentID& pageId,
                              TabPageSpec::GetModelExpression modelGetter,
                              TabPageSpec::GetSpecExpression specGetter)
{
    getPages()->addPage (label, pageId, modelGetter, specGetter);
}

void UITabComposite::populateContents (int index)
{
    if (index < 0 || index >= getPages()->size())
        return;
    
    const TabPageSpec& page = getPages()->getReference(index);
    
    if (notificationPre != nullptr)
        notificationPre();
    
    if (page.getModel.getObject() == nullptr)
    {
        // Local UIModel
        UISpec* spec = UIAdaptor::getModel()->getClass()->getDefaultSpec();
        if (page.getSpec.getObject() != nullptr)
            spec = page.getSpec.getObject()(getModel()->getClass());
        
        UIAdaptor::getModel()->populateComposite (tabContentsRef, spec);
        
    } else {
        // External UIModel or EmbeddedUIModel
        UIModel* externalModel = page.getModel.getObject()(getModel());
        if (externalModel == nullptr)
            return;
        
        UISpec* spec = externalModel->getClass()->getDefaultSpec();
        if (page.getSpec.getObject() != nullptr)
            spec = page.getSpec.getObject()(getModel()->getClass());
        
        externalModel->populateComposite (tabContentsRef, spec);
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
    
    void UITabComposite::setComponentState (const Binding::Purpose& p, std::shared_ptr<TabPageList> contents)
{
    if (p == Binding::Purpose::GetValue)
    {
        pages = contents;
        buildPages();
        return;
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
        value = getPages()->getReference (currentPopulatedIndex).label;
        return;
    }
    UIAdaptor::getComponentState (p, value);
}

void UITabComposite::setComponentState (const Binding::Purpose& p, const var& value)
{
    if (p == Binding::Purpose::GetSelection)
        return setSelectedIndex (value);
    
    if (p == Binding::Purpose::GetLabel)
    {
        getPages()->getReference (currentPopulatedIndex).label = value.toString();
        tabBarRef->setTabName (currentPopulatedIndex, getPages()->getReference (currentPopulatedIndex).label);
        return;
    }
    UIAdaptor::setComponentState (p, value);
}

}
