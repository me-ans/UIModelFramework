
/**
 This file is automatically generated by UIEditor. It defines the layout and data bindings of
 all UISpec owned by the associated UIModel. Include the UIEditor module with your development
 build in order to browse, open and edit all UISpec in your application at runtime.
 
 (Last generated 28 Apr 2019 8:48:31pm)
 */

#include "CustomExample.h"

INIT_SYMBOL (CustomExample, Settings);
INIT_SYMBOL (CustomExample, Colours);

WeakReference<UISpec> CustomExample::DefaultSpec = new UISpec
(
 CustomExample::Class::instance(),
 "DefaultSpec",
 []()
 {
    auto part1 = new CompositeSpec ("part1");
    {
        auto l1 = new LabelSpec ("l1");
        l1->setLayout (LayoutFrame("4", "4", "100% - 4", "40 relative"));
        l1->setLabel ("Canvas component populated with LayoutCursor");
        part1->addComponent (l1);
    }
    {
        auto swatches = new CanvasSpec ("swatches");
        swatches->setLayout (LayoutFrame("4", "50", "100% - 200", "100% - 4"));
        swatches->addBinding (Bind::Canvas (MEMBER (&CustomExample::populateCustomComponent), Canvases));
        part1->addComponent (swatches);
    }
    {
        auto user = new UserDefinedSpec ("user", "ColourSelector()", []() { return new ColourSelector(); });
        user->setLayout (LayoutFrame("100% - 200", "50", "100% - 4", "100% - 4"));
        part1->addComponent (user);
    }
    return part1;
},
true
);



