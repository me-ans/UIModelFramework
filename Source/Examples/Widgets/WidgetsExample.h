/**
 Experimental UI Framework
 
 A proof-of-concept for a declarative UI framework based on generated code.
 Please regard the license terms accompanying this project.
 Copyright 2019 me-ans@GitHub
 */

#pragma once

#include "JuceHeader.h"

class WidgetsExample   : public EmbeddedUIModel
{
public:
    
#include "WidgetsExample.specs.h"
    
    WidgetsExample (UIModel* parent) :
        EmbeddedUIModel(parent),
        list (false, false)
    {
        list.setListData ({ "Vanilla", "Chocolate", "Strawberry", "Orange", "Butter Pecan", "Almond" });
        list.setSelectedIndex (2);
    }
    
   ~WidgetsExample () {}
    
    // ------------------------------------------------------------------------------
    double getProgress() { return progress; }
    
    // ------------------------------------------------------------------------------
    double getSlider() { return sliderValue; }
    void setSlider (double value)
    {
        sliderValue = value;
        changed (Settings);
    }

    // ------------------------------------------------------------------------------
    UIListModel<String>& getListContents () { return list; }
    Selection&           getListSelection() { return list.getSelection(); };
    void                 setListSelection (Selection& selection)
    {
        list.setSelection (selection);
        if (auto item = list.getSingleSelection())
            comments << "\nSelected flavour " << *item;
        changed ({Transcript, Settings});
    };

    // ------------------------------------------------------------------------------
    int  getRadioSelection () { return selectedRadioButton; }
    void setRadioSelection (int newSelection)
    {
        selectedRadioButton = newSelection;
        comments << "\nSelected radio " << selectedRadioButton;
        changed ({Transcript, Settings});
    }
    
    // ------------------------------------------------------------------------------
    UIValueMap getComboContents ()
    {
        UIValueMap m;
        m.add ({1, "One"});
        m.add ({2, "Two"});
        m.add ({3, "Three"});
        m.add ({-1, "Heading"});
        m.add ({16, "Sixteen"});
        m.add ({32, "Thirtytwo"});
        return m;
    }
    
    int  getComboSelection() { return selectedComboId; };
    void setComboSelection (int selection)
    {
        selectedComboId = selection;
        comments << "\nSelected combo id " << selectedComboId;
        changed ({Transcript, Settings});
    }
    void setComboText (const String& edited)
    {
        comments << "\nEdited combo text " << edited;
        changed ({Transcript, Settings});
    };
    
    // ------------------------------------------------------------------------------
    bool getCommentsVisible()   { return true; }
    bool getCommentsEnabled()   { return commentsEnabled; }
    String getCommentsContents()  { return comments; }
    void setCommentsContents (String& text) { comments = text; }
    
    void setupComments (Component* editor)
    {
        if (auto e = dynamic_cast<TextEditor*>(editor))
        {
            auto fg = (commentsEnabled ? Colours::white : Colours::darkgrey);
            e->setColour (TextEditor::backgroundColourId, (commentsEnabled ? Colours::darkgrey : Colours::black));
            e->setColour (TextEditor::textColourId, fg);
            e->applyColourToAllText (fg);
            e->repaint();
        };
    }
    
    // ------------------------------------------------------------------------------
    const String  getHideButtonLabel() { return commentsEnabled ? "Disable" : "Enable Again"; }
    void wasHideButtonClicked()
    {
        commentsEnabled = !commentsEnabled;
        changed (Visibility);
    }
    
    // ------------------------------------------------------------------------------
    bool getCancelButtonEnabled() { return commentsEnabled; }
    void wasCancelButtonClicked()
    {
        comments << "\nClicked Cancel";
        changed (Transcript);
    }
    
    // ------------------------------------------------------------------------------
    bool getOkButtonEnabled() { return commentsEnabled; }
    void wasOkButtonClicked()
    {
        comments << "\nClicked OK";
        progress += 0.1;
        if (progress >= 1.0) progress = 0.0;
        changed ({Transcript, Progress});
    }
    
private:

    String  comments = "Some random initial comment.";
    bool    commentsEnabled = true;
    int     selectedRadioButton = 1;
    int     selectedComboId = 16;
    
    UIListModel<String> list;

    double  progress = 0.1;
    double  sliderValue = 0.5;
};
