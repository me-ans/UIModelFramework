// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.

//#include "AppConfig.h"

#include "JuceHeader.h"

#include "utility/ans_LayoutFrame.cpp"
#include "utility/ans_LayoutCursor.cpp"

#include "core/ans_Binding.cpp"
#include "adaptors/ans_adaptor_base.cpp"

#include "specs/ans_UISpec.cpp"

#include "adaptors/ans_adaptor_composite.cpp"
#include "adaptors/ans_adaptor_lists.cpp"
#include "adaptors/ans_adaptor_tabs.cpp"
#include "adaptors/ans_adaptor_text.cpp"
#include "adaptors/ans_adaptor_tree.cpp"
#include "adaptors/ans_adaptor_widgets.cpp"
#include "adaptors/ans_adaptor_window.cpp"

#include "specs/ans_ComponentSpec.cpp"
#include "specs/ans_UIComponentClass.cpp"
#include "specs/ans_UITabsSpec.cpp"

#include "core/ans_UIBuilder.cpp"
#include "core/ans_UIInstance.cpp"
#include "core/ans_UIModel.cpp"

#include "editor/ans_UIEditor.cpp"
#include "editor/ans_UIEditor.specs.cpp"
#include "editor/inspector/ans_ComponentSpecInspector.cpp"
#include "editor/inspector/ans_ComponentSpecInspector.specs.cpp"
#include "editor/inspector/details/ans_LayoutSpecInspector.cpp"
#include "editor/inspector/details/ans_LayoutSpecInspector.specs.cpp"


