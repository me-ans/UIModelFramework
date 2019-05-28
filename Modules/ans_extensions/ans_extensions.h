/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:               ans_extensions
  vendor:           me-ans
  version:          0.1.0
  name:             Class Extensions For Juce
  description:      Metaclass, Symbol, Aspect and Model
  website:          https://github.com/me-ans/UIModelFramework
  license:          Proprietary

  dependencies:     juce_gui_extra

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#define ANS_EXTENSION_H_INCLUDED

#if ! JUCE_PROJUCER_LIVE_BUILD

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>


/** Config: ANS_INCLUDE_NAMESPACE
    If your code uses a lot of ANS classes, keep this enabled to have the ans namespace included by default
 */
#ifndef ANS_INCLUDE_NAMESPACE
#define ANS_INCLUDE_NAMESPACE 1
#endif

/** Config: ANS_DEBUG_UPDATES
    Log change propagation of class Model to the console for debugging
*/
#ifndef ANS_DEBUG_UPDATES
 #define ANS_DEBUG_UPDATES 0
#endif


// Useful for debugging
#define ANS_CLASSNAME String(typeid(*this).name()).removeCharacters("0123456789_")

/*
namespace ans
{
 class Metaclass;
 class Symbol;
 class SymbolTable;
 class SymbolTableEntry;
 class Aspects;
 class Dependent;
 class Model;
 class Printable;
}
 */

#include "core/ans_Metaclass.h"
#include "core/ans_Symbol.h"
#include "core/ans_Model.h"
#include "core/ans_Printable.h"

#if ANS_INCLUDE_NAMESPACE
using namespace ans;
#endif

#endif
