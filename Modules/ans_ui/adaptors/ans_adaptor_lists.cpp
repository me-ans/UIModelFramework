/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_lists.h"
#include "../core/ans_UIModel.h"

namespace ans {
    using namespace juce;

//==========================================================================================================
#if 0
#pragma mark UIListBox
#endif

UIListBox::UIListBox (std::shared_ptr<UIInstance> instance, const ListSpec& spec) :
    ListBox (spec.identifier),
    UIAdaptor (instance, spec),
    listModel (nullptr)
{
    initialiseFromSpec (instance, spec);
    ListBox::setMultipleSelectionEnabled (spec.enableMultipleSelection);
    ListBox::setModel (nullptr);
}

UIListBox::~UIListBox ()
{
    if (listModel != nullptr)
        listModel->removeView (this);
    
    ListBox::setModel (nullptr);
}

void UIListBox::setComponentState (const Binding::Purpose& p, UIListModelBase& contents)
{
    if (p == Binding::Purpose::GetValue)
    {
        listModel = &contents;
        listModel->addView (this);
        // @todo: Issue a warning if mismatched with spec!
        ListBox::setMultipleSelectionEnabled (listModel->getSelection().isMultipleSelectionEnabled());
        
        if (ListBox::getModel() != listModel)
            ListBox::setModel (listModel);
        else
        {
            ListBox::repaint();
            ListBox::updateContent();
        }
    }
}

void UIListBox::getComponentState (const Binding::Purpose& p, Selection& selection)
{
    if (p == Binding::Purpose::SetSelection)
    {
        selection = currentSelection();
        // not here, as UIModel needs to do so in response
        //listModel->setSelection (selection);
    }
}

void UIListBox::setComponentState (const Binding::Purpose& p, Selection& selection)
{
    if (p == Binding::Purpose::GetSelection)
    {
        SparseSet<int> newSelection;
        for (auto i : selection.getMulti())
            newSelection.addRange(Range<int>(i, i+1));
        
        setSelectedRows (newSelection, dontSendNotification);
    }
}

Selection UIListBox::currentSelection()
{
    Selection selection;
    for (int i = 0; i < getNumSelectedRows(); ++i)
        selection.setSelectionOf (getSelectedRow (i), true);
    return selection;
}

void UIListBox::someViewChangedSelection()
{
    /*
     NOTE: This is a bad hack, since we don't know WHICH view changed selection
     (Juce assumes all list models have only one view open). Therefore we need to
     somehow check if this originated from this view, so we don't notify UIListModel
     multiple times about the change.
     */
    
    if (listModel == nullptr || listModel->getSelection() != currentSelection())
        performBinding (Binding::Purpose::SetSelection);
}

}

