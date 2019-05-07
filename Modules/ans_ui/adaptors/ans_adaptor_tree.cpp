/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_tree.h"

namespace ans {
    using namespace juce;


void UITreeModelBase::selectionChanged ()
{
    views.call ([](auto& view) { view.performBinding (Binding::Purpose::SetSelection); });
}


void UITreeItemBase::itemSelectionChanged (bool nowSelected)
{
    if (auto view = dynamic_cast<UITreeView*> (getOwnerView()))
        view->itemSelectionChanged (this, nowSelected);
}


//==========================================================================================================
#if 0
#pragma mark UITreeView
#endif

UITreeView::UITreeView (std::shared_ptr<UIInstance> instance, const TreeSpec& spec) :
    TreeView (spec.identifier),
    UIAdaptor (instance, spec),
    treeModel (nullptr)
{
    initialiseFromSpec (instance, spec);
    
    setRootItemVisible (true);
    setMultiSelectEnabled (spec.enableMultipleSelection); // still depends on Selection being provided
    setOpenCloseButtonsVisible (true);
    setDefaultOpenness (true);
}

UITreeView::~UITreeView ()
{
    if (treeModel != nullptr)
        treeModel->removeView (this);
    
    setRootItem (nullptr);
}

void UITreeView::setComponentState (const Binding::Purpose& p, UITreeModelBase& contents)
{
    if (p == Binding::Purpose::GetValue)
    {
        treeModel = &contents;
        setRootItem (treeModel->getRootItem());
        // @todo: Issue a warning if mismatched with spec!
        setMultiSelectEnabled (treeModel->getSelection().isMultipleSelectionEnabled());
        treeModel->addView (this);
    }
}

void UITreeView::getComponentState (const Binding::Purpose& p, Selection& selection)
{
    if (p == Binding::Purpose::SetSelection)
    {
        selection.clear();
        for (int i = 0; i < getNumSelectedItems(); ++i)
            if (auto item = dynamic_cast<UITreeItemBase*> (getSelectedItem (i)))
                selection.setSelectionOf (item->getIndex(), true);
    }
}

void UITreeView::setComponentState (const Binding::Purpose& p, Selection& selection)
{
    if (p == Binding::Purpose::GetSelection)
    {
        bool clear = true;
        enumerateTreeItems ([&](auto item)
        {
            item->setSelected (selection.contains (item->getIndex()), clear, dontSendNotification);
            clear = false;
        });
    }
}

void UITreeView::itemSelectionChanged (UITreeItemBase* selectedItem, bool nowSelected)
{
    if (nowSelected)
        performBinding (Binding::Purpose::SetSelection);
}

}
