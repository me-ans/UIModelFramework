
/**
 This file is automatically generated by UIEditor. It defines the layout and data bindings of
 all UISpec owned by the associated UIModel. Include the UIEditor module with your development
 build in order to browse, open and edit all UISpec in your application at runtime.
 
 (Last generated 29 Apr 2019 3:47:44pm)
 */

#include "WidgetsExample.h"


INIT_SYMBOL (WidgetsExample, Transcript);
INIT_SYMBOL (WidgetsExample, Progress);
INIT_SYMBOL (WidgetsExample, List);
INIT_SYMBOL (WidgetsExample, Settings);
INIT_SYMBOL (WidgetsExample, Colours);
INIT_SYMBOL (WidgetsExample, TabContents);
INIT_SYMBOL (WidgetsExample, TabSelection);

WeakReference<UISpec> WidgetsExample::DefaultSpec = new UISpec
(
 WidgetsExample::getMetaClass(),
 "DefaultSpec",
 []()
 {
    auto main = new CompositeSpec ("main");
    {
        auto part1 = new CompositeSpec ("part1");
        part1->setLayout (LayoutFrame("4", "40", "100% - 300 [280..600]", "100% - 4 [200..600]"));
        {
            auto comments = new TextSpec ("comments", Transcript);
            comments->setLayout (LayoutFrame("0", "0", "100%", "100% - 100"));
            comments->addBinding (Bind::GetValue (MEMBER (&WidgetsExample::getCommentsContents), Transcript));
            comments->addBinding (Bind::SetValue (MEMBER (&WidgetsExample::setCommentsContents)));
            comments->addBinding (Bind::GetEnabled (MEMBER (&WidgetsExample::getCommentsEnabled), Transcript));
            comments->addBinding (Bind::GetVisible (MEMBER (&WidgetsExample::getCommentsVisible), Transcript));
            comments->addBinding (Bind::Config (MEMBER (&WidgetsExample::setupComments), Visibility));
            part1->addComponent (comments);
        }
        {
            auto hideButton = new ButtonSpec ("hideButton");
            hideButton->setLayout (LayoutFrame("0", "100% - 90", "33% - 2", "22 relative"));
            hideButton->setLabel ("Disable");
            hideButton->addBinding (Bind::Action (MEMBER (&WidgetsExample::wasHideButtonClicked)));
            hideButton->addBinding (Bind::GetLabel (MEMBER (&WidgetsExample::getHideButtonLabel), Visibility));
            part1->addComponent (hideButton);
        }
        {
            auto cancelButton = new ButtonSpec ("cancelButton");
            cancelButton->setLayout (LayoutFrame("33%", "100% - 90", "66% - 2", "22 relative"));
            cancelButton->setLabel ("Cancel");
            cancelButton->addBinding (Bind::Action (MEMBER (&WidgetsExample::wasCancelButtonClicked)));
            cancelButton->addBinding (Bind::GetEnabled (MEMBER (&WidgetsExample::getCancelButtonEnabled), Visibility));
            part1->addComponent (cancelButton);
        }
        {
            auto okButton = new ButtonSpec ("okButton");
            okButton->setLayout (LayoutFrame("66%", "100% - 90", "100%", "22 relative"));
            okButton->setLabel ("OK");
            okButton->addBinding (Bind::Action (MEMBER (&WidgetsExample::wasOkButtonClicked)));
            okButton->addBinding (Bind::GetEnabled (MEMBER (&WidgetsExample::getOkButtonEnabled), Visibility));
            part1->addComponent (okButton);
        }
        {
            auto group1 = new GroupSpec ("group1");
            group1->setLayout (LayoutFrame("0", "100% - 60", "100%", "100%"));
            {
                auto rad1 = new RadioSpec ("rad1", Settings);
                rad1->setLayout (LayoutFrame("32", "20", "100 relative", "22 relative"));
                rad1->setLabel ("A");
                rad1->setGroup (1);
                rad1->setValue (1);
                rad1->setConnectedEdges (Button::ConnectedOnRight);
                rad1->addBinding (Bind::GetSelection (MEMBER (&WidgetsExample::getRadioSelection), Settings));
                rad1->addBinding (Bind::SetSelection (MEMBER (&WidgetsExample::setRadioSelection)));
                group1->addComponent (rad1);
            }
            {
                auto rad2 = new RadioSpec ("rad2");
                rad2->setLayout (LayoutFrame("132", "20", "100 relative", "22 relative"));
                rad2->setLabel ("B");
                rad2->setGroup (1);
                rad2->setValue (2);
                rad2->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnRight);
                group1->addComponent (rad2);
            }
            {
                auto rad3 = new RadioSpec ("rad3");
                rad3->setLayout (LayoutFrame("232", "20", "100 relative", "22 relative"));
                rad3->setLabel ("C");
                rad3->setGroup (1);
                rad3->setValue (3);
                rad3->setConnectedEdges (Button::ConnectedOnLeft);
                group1->addComponent (rad3);
            }
            part1->addComponent (group1);
        }
        main->addComponent (part1);
    }
    {
        auto part2 = new CompositeSpec ("part2");
        part2->setLayout (LayoutFrame("100% - 300 [280..600]", "40", "100% - 4 [500..800]", "100% - 4 [200..600]"));
        {
            auto progress = new ProgressSpec ("progress", Progress);
            progress->setLayout (LayoutFrame("4", "4", "100% - 4", "22 relative"));
            progress->setLabel ("Press OK Button");
            progress->addBinding (Bind::GetValue (MEMBER (&WidgetsExample::getProgress), Progress));
            part2->addComponent (progress);
        }
        {
            auto list = new ListSpec ("list", Settings);
            list->setLayout (LayoutFrame("4", "32", "100% - 4", "100% - 70"));
            list->addBinding (Bind::GetValue (MEMBER (&WidgetsExample::getListContents), List));
            list->addBinding (Bind::GetSelection (MEMBER (&WidgetsExample::getListSelection), Settings));
            list->addBinding (Bind::SetSelection (MEMBER (&WidgetsExample::setListSelection)));
            part2->addComponent (list);
        }
        {
            auto combo = new ComboSpec ("combo", Settings);
            combo->setLayout (LayoutFrame("4", "100% - 68", "100% - 4", "100% - 34"));
            combo->addBinding (Bind::GetValue (MEMBER (&WidgetsExample::getComboContents), Settings));
            combo->addBinding (Bind::SetValue (MEMBER (&WidgetsExample::setComboText)));
            combo->addBinding (Bind::GetSelection (MEMBER (&WidgetsExample::getComboSelection), Settings));
            combo->addBinding (Bind::SetSelection (MEMBER (&WidgetsExample::setComboSelection)));
            part2->addComponent (combo);
        }
        {
            auto slider = new SliderSpec ("slider", Settings);
            slider->setLayout (LayoutFrame("4", "100% - 32", "100% - 4", "100% - 4"));
            slider->setRange (0, 1, 0.1);
            slider->addBinding (Bind::GetValue (MEMBER (&WidgetsExample::getSlider), Settings));
            slider->addBinding (Bind::SetValue (MEMBER (&WidgetsExample::setSlider)));
            part2->addComponent (slider);
        }
        main->addComponent (part2);
    }
    return main;
},
true
);




