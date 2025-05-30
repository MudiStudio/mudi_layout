/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 mudi_layout
  vendor:             mudi
  version:            1.0.0
  name:               Mudi Layout
  description:        Give layout classes to easy create the app layout

  dependencies:       juce_core,juce_graphics,juce_gui_basics

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/
#pragma once
#define MUDI_LAYOUT_H_INCLUDED


#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#ifdef JUCE_MODULE_AVAILABLE_mudi_glfw
#include <mudi_glfw/mudi_glfw.h>
#endif // JUCE_MODULE_AVAILABLE_mudi_glfw

namespace mudi {

	namespace mudi_layout {

		using namespace juce;

		class BaseLayoutComponent;

		class LayoutHeaderPanelComponent;

		class Layout;
		class MudiLayoutManager;
		class DockManager;
		class StackLayout;
		class StretchLayout;
		class StretchMultiViewLayout;

	}

}


#include "Factory/include/Factory.h"
#include "Factory/include/FactoryDatabase.h"
#include "Factory/include/BaseFactoryDatabase.h"
#include "Factory/include/FactoryBuildable.h"

#include "BaseLayoutComponent/include/BaseLayoutComponent.h"

#include "LayoutComponents/include/LayoutHeaderPanelComponent.h"

#include "MudiLayoutManager/include/MudiLayoutManager.h"
#include "MudiLayoutManager/include/DockManager.h"
#include "MudiLayoutManager/include/StackLayout.h"
#include "MudiLayoutManager/include/StretchLayout.h"
#include "MudiLayoutManager/include/StretchMultiViewLayout.h"

#include "LayoutUtilities/include/LayoutUtilities.h"
#include "VTIdentifiers.h"