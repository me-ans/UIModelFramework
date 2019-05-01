/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

//#include "ans_Binding.h"
//#include "ans_UIModel.h"

namespace ans
{
    using namespace juce;

/**
 Initialisation of known binding purposes:
 As these are saved and parsed by name, this list can be extended at any time.
 */
const Binding::Purpose Binding::Purpose::GetEnabled   = {0,  Binding::Signature::Update_Component,  "GetEnabled",    false, true, UIModel::Visibility};
const Binding::Purpose Binding::Purpose::GetVisible   = {1,  Binding::Signature::Update_Component,  "GetVisible",    false, true, UIModel::Visibility};
const Binding::Purpose Binding::Purpose::GetValue     = {2,  Binding::Signature::Update_Component,  "GetValue",      true,  true, UIModel::Values};
const Binding::Purpose Binding::Purpose::GetSelection = {3,  Binding::Signature::Update_Component,  "GetSelection",  true,  true, UIModel::Selections};
const Binding::Purpose Binding::Purpose::GetLabel     = {4,  Binding::Signature::Update_Component,  "GetLabel",      true,  true, UIModel::Labels};
const Binding::Purpose Binding::Purpose::SetValue     = {5,  Binding::Signature::Update_Model,      "SetValue",      false};
const Binding::Purpose Binding::Purpose::SetSelection = {6,  Binding::Signature::Update_Model,      "SetSelection",  false};
const Binding::Purpose Binding::Purpose::SetLabel     = {7,  Binding::Signature::Update_Model,      "SetLabel",      false};
const Binding::Purpose Binding::Purpose::Action       = {8,  Binding::Signature::Trigger_Action,    "Action",        false};
const Binding::Purpose Binding::Purpose::Config       = {9,  Binding::Signature::Pass_Component,    "Config",        false};
const Binding::Purpose Binding::Purpose::Layout       = {10, Binding::Signature::Pass_Component,    "Layout",        false};
const Binding::Purpose Binding::Purpose::Canvas       = {11, Binding::Signature::Populate_Composite,"Canvas",        false, true, UIModel::Canvases};

// This array builds up dynamically at runtime when the above globals get automatically initialised
Array<Binding::Purpose> Binding::Purpose::KnownTypes = {};

//==========================================================================================================
#if 0
#pragma mark Binding::Purpose
#endif

Binding::Purpose::Purpose (int key_,
                           Signature signature_,
                           const String& name_,
                           bool userDefinedAspectRequired_,
                           bool respondToUndefined_,
                           Aspect defaultAspect_) :
    key (key_),
    signature (signature_),
    name (name_),
    userDefinedAspectRequired (userDefinedAspectRequired_),
    respondToUndefined (respondToUndefined_),
    defaultAspect (defaultAspect_)
{
   if (!KnownTypes.contains(*this))
       KnownTypes.add (*this);
}

const Binding::Purpose Binding::Purpose::parse (const String& name)
{
    for (auto t : Binding::Purpose::KnownTypes)
        if (t.name == name)
            return t;
    
    DBG ("*** ERROR: Unknown binding type " << name.quoted());
    jassertfalse;
    return Binding::Purpose::Action;
}


//==========================================================================================================
#if 0
#pragma mark Binding
#endif

String Binding::generateSourceCPP (Model::Class* modelClass) const
{
    String aspectDef;
    if (isAspected() && aspect != Model::Undefined)
        aspectDef << ", " << modelClass->printAspect(aspect);
    
    String out;
    out << generateSourceTemplate()
            .replace("$CLASS", modelClass->getName())
            .replace("$PURPOSE", purpose.name)
            .replace("$SOURCE", source)
            .replace("$ASPECT", aspectDef);
    
    return out;
}

}
