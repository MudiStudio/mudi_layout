#ifdef MUDI_LAYOUT_H_INCLUDED
/* When you add this cpp file to your project, you mustn't include it in a file where you've
   already included any other headers - just put it inside a file on its own, possibly with your config
   flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
   header files that the compiler may be using.
*/
#error "Incorrect use of JUCE cpp file"
#endif


#include "mudi_layout.h"

#include "LayoutUtilities/LayoutUtilities.cpp"

#include "LayoutComponents/LayoutHeaderPanelComponent.cpp"

#include "BaseLayoutComponent/BaseLayoutComponent.cpp"

#include "Factory/BaseFactoryDatabase.cpp"
#include "Factory/Factory.cpp"


#include "MudiLayoutManager/DockManager.cpp"
#include "MudiLayoutManager/MudiLayoutManager.cpp"
#include "MudiLayoutManager/StackLayout.cpp"
#include "MudiLayoutManager/StretchLayout.cpp"
#include "MudiLayoutManager/StretchMultiViewLayout.cpp"
