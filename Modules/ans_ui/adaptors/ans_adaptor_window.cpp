/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#include "ans_adaptor_window.h"
#include "ans_adaptor_composite.h"
#include "../core/ans_UIModel.h"

namespace ans {
    using namespace juce;

//==========================================================================================================
#if 0
#pragma mark UIWindowBase
#endif

UIWindowBase::UIWindowBase (UIInstance* instance, const WindowSpec& spec) :
    UIAdaptor (instance, spec),
    look (new LookAndFeel_V4())
{
    uiInstance = instance; // takes ownership here, while other adaptors have a weak reference only
}

UIWindowBase::~UIWindowBase()
{
}

void UIWindowBase::close()
{
    auto model = dynamic_cast<WindowUIModel*>(getModel());
    UIInstance::Ptr backup = uiInstance;
    
    delete this;
  
    if (model != nullptr && backup != nullptr)
        model->postClose (*backup);
}

void UIWindowBase::update (Model* sender, Aspect aspect, void* argument)
{
    if (aspect == WindowUIModel::WindowClose)
        return close();
    
    if (aspect == WindowUIModel::WindowLayout)
        return updateLayoutFromSpec();

    UIAdaptor::update (sender, aspect, argument);
}

void UIWindowBase::updateLayoutFromSpec()
{
    uiInstance.get()->updateLayoutFromSpec();
}




//==========================================================================================================
#if 0
#pragma mark UIDocumentWindow
#endif

UIDocumentWindow::UIDocumentWindow (UIInstance* instance, const WindowSpec& spec) :
    DocumentWindow (spec.label,
                    Desktop::getInstance().getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons,
                    true),
    UIWindowBase (instance, spec)
{
    initialiseFromSpec (instance, spec);
    
    setUsingNativeTitleBar (true);
    setResizable (true, false);
    setLookAndFeel (look);
}

UIDocumentWindow::~UIDocumentWindow()
{
    setLookAndFeel (nullptr);
    
    if (auto content = getContentComponent())
    {
        setContentNonOwned (nullptr, false);
        delete content;
    }
}

UIComposite* UIDocumentWindow::getUIComposite()
{
    return dynamic_cast<UIComposite*>(getContentComponent());
}


void UIDocumentWindow::resized()
{
    DocumentWindow::resized();
    
    if (auto content = getContentComponent())
        if (auto positioner = content->getPositioner())
            positioner->applyNewBounds (getBounds());
}

void UIDocumentWindow::closeButtonPressed ()
{
    if (auto model = dynamic_cast<WindowUIModel*>(getModel()))
    {
        if (model->canCloseWindow())
            close();
    }
}


}
