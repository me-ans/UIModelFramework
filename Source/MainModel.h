/**
 Experimental UI Framework
 
 A proof-of-concept for a declarative UI framework based on generated code.
 Please regard the license terms accompanying this project.
 Copyright 2019 me-ans@GitHub
 */

#pragma once

#include "JuceHeader.h"
#include "Examples/Widgets/WidgetsExample.h"
#include "Examples/Custom/CustomExample.h"

class MainModel : public WindowUIModel
{
public:
#include "MainModel.specs.h"    
    
    MainModel() : widgetsExample(this), customExample(this) {}
   ~MainModel() {}
    
    const String getWindowTitle() override { return String("UIModel Example"); }
    
    int  getTabSelection () { return selectedTabIndex; }
    void setTabSelection (int selection) { selectedTabIndex = selection; }
    
    // Example how to build tabs programmatically that use embedded sub-models
    
    TabPageList::Shared getTabPages ()
    {
        if (tabs != nullptr)
            return tabs;
        
        tabs = std::make_shared<TabPageList>();
        
        tabs->addPage ("Widgets",
                       "tabWidgets",
                       FROM_SOURCE ([&](UIModel* m){ return &widgetsExample; }),
                       FROM_SOURCE ([&](Model::Class* mc){ return WidgetsExample::DefaultSpec; }));
        tabs->addPage ("Canvas Component",
                       "tabCanvas",
                       FROM_SOURCE ([&](UIModel* m){ return &customExample; }),
                       FROM_SOURCE ([&](Model::Class* mc){ return CustomExample::DefaultSpec; }));
        return tabs;
    }
    
    void postBuild (UIInstance& instance) override
    {
        if (instance.isForWindow())
            changed (TabContents);
        
        Parent::postBuild (instance);
    }
    
private:
    
    TabPageList::Shared tabs;
    int selectedTabIndex = 0;
    WidgetsExample widgetsExample;
    CustomExample  customExample;
    
};
