/**
 Experimental UI Framework
 
 A proof-of-concept for a declarative UI framework based on generated code.
 Please regard the license terms accompanying this project.
 Copyright 2019 me-ans@GitHub
 */

#pragma once

#include "JuceHeader.h"

class CustomExample : public EmbeddedUIModel
{
public:
#include "CustomExample.specs.h"
    
    CustomExample (UIModel* parent) : EmbeddedUIModel(parent) {}
   ~CustomExample () {}
    
    void populateCustomComponent (UIComposite* comp);
    
private:

    
};
