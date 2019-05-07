/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

#include "ans_adaptor_base.h"
#include "ans_adaptor_composite.h"
#include "../specs/ans_UITabsSpec.h"

namespace ans {
    using namespace juce;
    
/**
 UITabComposite resembles TabbedComponent to some extent, adding full support for
 communication with UIModel. Instead of supplying tab pages at build time already,
 which has several disadvantages, this class supports dynamically adding any UISpec
 of any UIModel as a tab page.
 
 @todo: The getters/setters used for UIModel/UISpec access are somewhat awkward ...
 */
    
class UITabComposite :
        public UIComposite,
        public ChangeListener
{
public:
        
    UITabComposite (std::shared_ptr<UIInstance> instance, const TabsSpec& spec);
   ~UITabComposite ();
    
    UIComposite* getUIComposite() override { return tabContentsRef; };
    
    void deleteContents() override;
    
    int  getSelectedIndex ();
    void setSelectedIndex (int index);
    
    ComponentID getSelectedID ();
    void setSelectedID (const ComponentID& name);
    
    void clearPages();
    void buildPages();
    
    void addPage (const String& label,
                  const ComponentID& pageId,
                  TabPageSpec::GetModelExpression modelGetter,
                  TabPageSpec::GetSpecExpression specGetter);
    
    std::shared_ptr<TabPageList> getPages();
    
    void setComponentState (const Binding::Purpose& p, std::shared_ptr<TabPageList> contents) override;
    void getComponentState (const Binding::Purpose& p, var& value) override;
    void setComponentState (const Binding::Purpose& p, const var& value) override;
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
private:
    void populateContents (int index);
    
    std::shared_ptr<TabPageList> pages;
    int defaultIndex;
    ComponentID defaultID;
    std::function<void()> notificationPre;
    std::function<void()> notificationPost;
    std::function<void()> confirmation;
    bool inhibit;
    int  currentPopulatedIndex;
    TabbedButtonBar* tabBarRef;
    UIComposite* tabContentsRef;
};

}
