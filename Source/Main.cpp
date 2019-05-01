/**
 Experimental UI Framework
 
 A proof-of-concept for a declarative UI framework based on generated code.
 Please regard the license terms accompanying this project.
 Copyright 2019 me-ans@GitHub
 */

#include "MainModel.h"


class UserInterfaceExampleApplication  :
        public JUCEApplication
{
public:
    UserInterfaceExampleApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    void initialise (const String& commandLine) override
    {
        model.initialise (commandLine);
        model.open();
    }

    void shutdown() override
    {
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
    }

private:
    MainModel model;
};


START_JUCE_APPLICATION (UserInterfaceExampleApplication)
