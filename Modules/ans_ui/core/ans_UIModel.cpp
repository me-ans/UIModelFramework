/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

namespace ans
{
    using namespace juce;

    INIT_SYMBOL (UIModel, Configs);
    INIT_SYMBOL (UIModel, Canvases);
    INIT_SYMBOL (UIModel, Visibility);
    INIT_SYMBOL (UIModel, Enablement);
    INIT_SYMBOL (UIModel, Labels);
    INIT_SYMBOL (UIModel, Values);
    INIT_SYMBOL (UIModel, Selections);
    INIT_SYMBOL (UIModel, WindowLayout);
    INIT_SYMBOL (UIModel, WindowClose);

    //==========================================================================================================
    #if 0
    #pragma mark UIModel
    #endif

    WeakReference<UISpec> UIModel::DefaultSpec = new UISpec
    (
     UIModel::getMetaClass(),
     "DefaultSpec",
     []()
     {
        auto content = new CompositeSpec ("content");
        content->setLayout (LayoutFrame::entire());
        return content;
     },
     true);


    UIModel::UIModel ()
    {
    }

    UIModel::~UIModel ()
    {
        masterReference.clear();
    }

    Identifier UIModel::getUniqueID ()
    {
        if (!identifier.isNull())
            return identifier;
        
        String id = getClass()->getName() + String((int64)this);
        auto parent = getParentModel();
        if (parent == nullptr)
            return identifier = "/" + id;
        else
            return identifier = parent->getUniqueID() + "/" +  id;
    }

    bool UIModel::populateComposite (UIComposite* composite, UISpec* spec)
    {
        if (composite == nullptr || spec == nullptr)
            return false;
        
        return UIBuilder::buildInto (spec, this, composite);
    }

    bool UIModel::populateComposite (UIComposite* composite)
    {
        return populateComposite (composite, getClass()->getDefaultSpec());
    }

    void UIModel::postBuild (UIInstance& instance)
    {
        changed (Visibility);
        changedAspects (instance.getSpec()->getUsedAspects());
    }

    void UIModel::openUIEditor()
    {
        auto editor = UIEditor::getInstance();
        editor->setUISpec (this->getClass()->getDefaultSpec());
        if (!editor->hasOpenWindows())
            editor->open();
    }

    void UIModel::openClone ()
    {
        if (auto app = dynamic_cast<WindowUIModel*> (topLevelModel()))
            app->open();
    }

    //==========================================================================================================
    #if 0
    #pragma mark WindowUIModel
    #endif

    WindowUIModel::WindowUIModel (WindowUIModel * master) : masterWindowModel (master)
    {
    }

    WindowUIModel::~WindowUIModel ()
    {
        changed (WindowClose); // causes all window adaptors to close their windows
    }

    void WindowUIModel::initialise (const String& commandLine)
    {
    }

    std::unique_ptr<TopLevelWindow> WindowUIModel::createWindow (UISpec* spec, bool open, const Rectangle<int>* frame)
    {
        if (spec == nullptr)
            return nullptr;
        
        auto window = UIBuilder::buildWindow (spec, this);
        if (open)
        {
            window->setVisible(true);
            if (frame != nullptr)
                window->setBounds (*frame);
            window->addToDesktop();
        }
        return window;
    }

    void WindowUIModel::open (UISpec* spec)
    {
        createWindow (spec).release();
    }

    void WindowUIModel::open (UISpec* spec, const Rectangle<int>& frame)
    {
        createWindow (spec, true, &frame).release();
    }

    void WindowUIModel::open ()
    {
        createWindow (getClass()->getDefaultSpec(), true).release();
    }

    void WindowUIModel::closeAllWindows()
    {
        changed (WindowClose);
    }

    void WindowUIModel::postOpen (UIInstance& instance)
    {
        
    }

    void WindowUIModel::postClose (UIInstance& instance)
    {
        //if (masterWindowModel == nullptr && !hasOpenWindows())
        //    delete this;
        
        if (TopLevelWindow::getNumTopLevelWindows() == 0)
            return JUCEApplication::getInstance()->systemRequestedQuit();
    }

    void WindowUIModel::openClone ()
    {
        open();
    }
    
    bool WindowUIModel::hasOpenWindows()
    {
        for (int i = TopLevelWindow::getNumTopLevelWindows(); --i >= 0;)
            if (auto win = dynamic_cast<UIWindowBase*> (TopLevelWindow::getTopLevelWindow (i)))
                if (win->getModel() == this)
                    return true;
        
        return false;
    }

    //==========================================================================================================
    #if 0
    #pragma mark UIModel::Class
    #endif

    void UIModel::Class::setAspectNames (const Array<String>& editedNames)
    {
        changedAspectNames = editedNames;
    }

    void UIModel::Class::addSpec (UISpec* spec)
    {
        if (specs.get() == nullptr)
            specs = new UISpecRegistry();
        
        specs->addSpec (spec);
    }

    Array<UISpec*> UIModel::Class::getUISpecs()
    {
        if (specs.get() == nullptr)
            specs = new UISpecRegistry();
        
        return specs->getUISpecs();
    }

    UISpec* UIModel::Class::getDefaultSpec()
    {
        UISpec* any = nullptr;
        for (auto each : getUISpecs())
        {
            any = each;
            if (each->isDefault())
                return each;
        }
        
        if (any == nullptr)
        {
            if (auto parent = dynamic_cast<UIModel::Class*>(getSuperClass()))
                return parent->getDefaultSpec();
        }
        
        return any;
    }
    
    File UIModel::Class::getSpecsFile() const
    {
        const String raw = getSpecsFilename();
        if (raw.indexOf("..") == 0)
            // Relative "../../Modules/ans_ui/adaptors/../core/ans_UIModel.h"
            return File (ANS_PROJECT_DIR).getChildFile (raw);
        else
            // Absolute
            return File (raw);
    }
    
    const String UIModel::Class::getTemplate (const String& name)
    {
        File file = UIModel::getMetaClass()->getSpecsFile().getParentDirectory().getChildFile("templates").getChildFile(name);
        if (!file.existsAsFile())
        {
            // If you get an assertion here, you'll need to include preprocessor macro PROJECT_DIR
            DBG ("*** ERROR: Could not find " << file.getFullPathName());
            jassertfalse;
        }
        return file.loadFileAsString();
    }
    
    void UIModel::Class::generateSourceFiles ()
    {
        if (!isEditable())
            return;
        
        File folder = getSpecsFile().getParentDirectory();
        generateSourceFiles (folder);
    }


    void UIModel::Class::generateSourceFiles (const File& targetFolder)
    {
        if (!isEditable())
            return;
        
        targetFolder.createDirectory();
        File baseFn = targetFolder.getChildFile (getSpecsFile().getFileName());
        
        File header (baseFn.withFileExtension (".h"));
        if (header.exists())
            header.copyFileTo (header.withFileExtension(".bak.h"));
        DBG ("Saving " << header.getFullPathName());
        header.replaceWithText (generateSourceHPP());
        
        File source (baseFn.withFileExtension (".cpp"));
        if (source.exists())
            source.copyFileTo (source.withFileExtension(".bak.cpp"));
        DBG ("Saving " << source.getFullPathName());
        source.replaceWithText (generateSourceCPP());
    }

    StringArray UIModel::Class::generateAspectNames ()
    {
        StringArray aspectNames = changedAspectNames;
        if (aspectNames.isEmpty())
            aspectNames = getAspectNames();
        
        return aspectNames;
    }

    String UIModel::Class::generateSourceCPP ()
    {
        if (!isEditable())
            return getName() + " is not editable";
        
        String ns1;
        String ns2;
        if (! getNameSpace().isEmpty())
        {
            ns1 = "namespace " + getNameSpace() + " {\nusing namespace juce;\n\n";
            ns2 = "} // namespace\n";
        }
        
        String symbolDefs;
        StringArray aspectNames = generateAspectNames();
        for (auto n : aspectNames)
            symbolDefs << "INIT_SYMBOL (" << getName() << ", " << n << ");\n";
        
        String specsSources;
        for (auto* specDef : getUISpecs())
            specsSources << specDef->generateSourceCPP (this);

        return getTemplate ("generate_UIModel_spec_cpp.h")
                .replace ("$TIME", Time::getCurrentTime().toString(true, true))
                .replace ("$MODEL", getName())
                .replace ("$INCLUDE", getSpecsFile().getFileName().upToFirstOccurrenceOf(".", false, true) + ".h")
                .replace ("$NS1", ns1)
                .replace ("$NS2", ns2)
                .replace ("$SYMBOLS", symbolDefs)
                .replace ("$SPECS", specsSources);
    }

    String UIModel::Class::generateSourceHPP ()
    {
        if (!isEditable())
            return getName() + " is not editable";
        
        String namespaceDef;
        String ns = getNameSpace();
        if (ns > "")
            namespaceDef = "\nconst String getNameSpace() override { return " + ns.quoted() + "; }\n";
       
        String symbolDefs;
        StringArray aspectNames = generateAspectNames();
        for (auto n : aspectNames)
            symbolDefs << "DEFINE_SYMBOL (" << n << ");\n";
        
        String globals;
        for (auto* specDef : getUISpecs())
            globals << "static WeakReference<UISpec> " << specDef->getName() << ";\n";

        return getTemplate ("generate_UIModel_spec_hpp.h")
                .replace ("$TIME", Time::getCurrentTime().toString(true, true))
                .replace ("$SYMBOLS", symbolDefs)
                .replace ("$ASPECTS", aspectNames.joinIntoString(", "))
                .replace ("$NS",      namespaceDef)
                .replace ("$MODEL",   getName())
                .replace ("$PARENT",  getSuperClass()->getName())
                .replace ("$GLOBALS", globals);
    }

} // namespace

