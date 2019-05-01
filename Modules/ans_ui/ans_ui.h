/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:               ans_ui
  vendor:           me-ans
  version:          0.1.0
  name:             Experimental UI Framework
  description:      Experimental UI Framework (proof-of-concept)
  website:          https://github.com/me-ans/UIModelFramework
  license:          Proprietary

  dependencies:     juce_gui_extra, ans_extensions

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#define ANS_UI_H_INCLUDED

#if ! JUCE_PROJUCER_LIVE_BUILD

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <ans_extensions/ans_extensions.h>

/** Config: ANS_INCLUDE_NAMESPACE
    If your code uses a lot of ANS classes, keep this enabled to have the ans namespace included by default
 */
#ifndef ANS_INCLUDE_NAMESPACE
#define ANS_INCLUDE_NAMESPACE 1
#endif

using namespace juce;

#ifndef ANS_PROJECT_DIR
#error You'll need to add preprocessor macro ANS_PROJECT_DIR for your platform
// Mac: ANS_PROJECT_DIR="$(PROJECT_DIR)"
// Win: ANS_PROJECT_DIR=R"\"($(ProjectDir))\""
// Linux: ???
#endif

#include "utility/ans_LayoutFrame.h"
#include "utility/ans_LayoutCursor.h"
#include "utility/ans_Positioners.h"
#include "utility/ans_SourceOutputStream.h"
#include "utility/ans_Tools.h"

#include "core/ans_Selection.h"
#include "core/ans_Binding.h"
#include "adaptors/ans_adaptor_base.h"
#include "core/ans_BindingTemplates.h"

#include "specs/ans_UISpec.h"

#include "adaptors/ans_adaptor_composite.h"
#include "adaptors/ans_adaptor_lists.h"
#include "adaptors/ans_adaptor_tabs.h"
#include "adaptors/ans_adaptor_text.h"
#include "adaptors/ans_adaptor_tree.h"
#include "adaptors/ans_adaptor_widgets.h"
#include "adaptors/ans_adaptor_window.h"

#include "specs/ans_ComponentSpec.h"
#include "specs/ans_UIComponentClass.h"
#include "specs/ans_UITabsSpec.h"

#include "core/ans_UIModel.h"
#include "core/ans_UIInstance.h"
#include "core/ans_UIBuilder.h"

#include "editor/ans_UIEditor.h"
#include "editor/inspector/ans_ComponentSpecInspector.h"
#include "editor/inspector/details/ans_LayoutSpecInspector.h"


#if ANS_INCLUDE_NAMESPACE
using namespace ans;
#endif

#endif
