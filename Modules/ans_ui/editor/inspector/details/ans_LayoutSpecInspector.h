/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

namespace ans {
    using namespace juce;

class ComponentSpecInspector;


/** LayoutSpecInspector edits a LayoutSpec. It offers the flexibility to support more types of layout, e.g. FlexBox */

class LayoutSpecInspector : public EmbeddedUIModel
{
public:
    
#include "ans_LayoutSpecInspector.specs.h"
    
    LayoutSpecInspector (ComponentSpecInspector* parent);
   ~LayoutSpecInspector () {}
    
    LayoutSpec& getLayoutSpec();
    
    String  getLeft ()                      { return getLayoutSpec().frame.left.print(); }
    void    setLeft (const String& input)   { getLayoutSpec().frame.left.parse(input); update(); }
    
    String  getTop ()                       { return getLayoutSpec().frame.top.print(); }
    void    setTop (const String& input)    { getLayoutSpec().frame.top.parse(input); update(); }

    String  getRight ()                     { return getLayoutSpec().frame.right.print(); }
    void    setRight (const String& input)  { getLayoutSpec().frame.right.parse(input); update(); }

    String  getBottom ()                    { return getLayoutSpec().frame.bottom.print(); }
    void    setBottom (const String& input) { getLayoutSpec().frame.bottom.parse(input); update(); }
    
    void update();
    
    void buildCanvas (UIComposite* canvas)
    {
        auto content = new CompositeSpec ("content");
        ScopedPointer<UISpec> ui = new UISpec (*getClass(), content);
        LayoutCursor cursor;
        populateInspector (content, cursor);
        populateComposite (canvas, ui);
        changed (LayoutSettings);
    }
    
    void populateInspector (CompositeSpec* composite, LayoutCursor& cursor);
    
    bool hasUIEditorLink() override { return false; }
    
private:
    
    ComponentSpecInspector* mainInspector;
    LayoutSpec dummy;
};

}
