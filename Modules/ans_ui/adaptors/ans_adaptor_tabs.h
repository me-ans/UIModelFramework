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
 UITabComposite resembles TabbedComponent to some extent, albeit with support
 for communication with UIModel. 
 */
class UITabComposite :
        public UIComposite,
        public ChangeListener
{
public:
        
    UITabComposite (UIInstance* owner, const TabsSpec& spec);
   ~UITabComposite ();
    
    UIComposite* getUIComposite() override { return tabContents; };
    
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
    
    
    void getComponentState (const Binding::Purpose& p, var& value) override;
    void setComponentState (const Binding::Purpose& p, const var& value) override;
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
private:
    void populateContents (int index);
    
    TabPageList pages;
    int defaultIndex;
    ComponentID defaultID;
    std::function<void()> notificationPre;
    std::function<void()> notificationPost;
    std::function<void()> confirmation;
    bool inhibit;
    int  currentPopulatedIndex;
    TabbedButtonBar* tabBar;  // merely a reference to a mandatory child of this UIComposite
    UIComposite* tabContents; // merely a reference to a mandatory child of this UIComposite
};

}
