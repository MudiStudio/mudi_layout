

#if 0
#include "StackLayout.cpp"
#include "StretchLayout.cpp"
#include "DockManager.cpp"
#include "StretchMultiViewLayout.cpp"  
#endif // 0

//#include "BinaryData.h"



namespace mudi {

	namespace mudi_layout
	{
		using namespace juce;

		Layout::Layout(ValueTree& state) : parent(nullptr),state(state)
		{
			
			setOpaque(true);

			colourWhenEmpty = Colour::fromHSV(Random::getSystemRandom().nextFloat(), 1.0f, 1.0f, 1.0f);
			
			id.referTo(state, VTIDs::idPropertyIdentifier, nullptr);

			jassert(id != "");
			setComponentID(id);

			iconName.referTo(state, VTIDs::iconNamePropertyIdentifier, nullptr);
			iconName.setDefault("");

			//TODO_UMB
            //String iconNameStr = iconName.get();
			//iconData = getBinaryRes(iconNameStr);

			isCollapsed.referTo(state, VTIDs::layoutCollapsedPropertyIdentifier, nullptr);
			isCollapsed.setDefault(false);

			isCollapsible.referTo(state, VTIDs::layoutCollapsiblePropertyIdentifier, nullptr);
			isCollapsible.setDefault(false);

			hasHeaderVisible.referTo(state, VTIDs::layoutHeaderVisibilityPropertyIdentifier, nullptr);
			hasHeaderVisible.setDefault(false);

			minSize.referTo(state, VTIDs::defminimumSizePropertyIdentifier, nullptr);
			minSize.setDefault(0);

			maxSize.referTo(state, VTIDs::defmaximumSizePropertyIdentifier, nullptr);
			maxSize.setDefault(0);

			prefSize.referTo(state, VTIDs::defPreferredSizePropertyIdentifier, nullptr);
			prefSize.setDefault(0);

			miniatureSlots.referTo(state, VTIDs::layoutMiniatureSlotsPropertyIdentifier, nullptr);
			miniatureSlots.setDefault(0);

			headerHeight.referTo( state, VTIDs::layoutHeaderHeightPropertyIdentifier, nullptr);
			headerHeight.setDefault(44);

			topMargin.referTo(state, VTIDs::topMarginPropertyIdentifier , 0);
			topMargin.setDefault(0);

			bottomMargin.referTo(state, VTIDs::bottomMarginPropertyIdentifier, 0);
			bottomMargin.setDefault(0);

			leftMargin.referTo(state, VTIDs::leftMarginPropertyIdentifier, 0);
			leftMargin.setDefault(0);

			rightMargin.referTo(state, VTIDs::rightMarginPropertyIdentifier, 0);
			rightMargin.setDefault(0);

			margin.referTo(state, VTIDs::marginPropertyIdentifier, 0);
			margin.setDefault(0);

		}

		Layout::~Layout() {
			for (auto* c : comps) {
				removeChildComponent(c);
			}
			comps.clear();
		}

		Component* Layout::findComponentByIdRecursive(const String& id, Layout* currentLayout) {

			static Component* find = nullptr;

			if (currentLayout == nullptr)
				currentLayout = this;

			for (auto* c : currentLayout->getItemsComponent()) {

				if (LayoutManager->isBaseLayoutComponent(c) && c->getComponentID().equalsIgnoreCase(id)) {
					find = LayoutManager->isBaseLayoutComponent(c);
				}

				if (LayoutManager->isLayoutComponent(c)) {
					findComponentByIdRecursive(id, LayoutManager->isLayoutComponent(c));
				}

			}

			return find;

		}

		Layout* Layout::findLayoutByComponentIdRecursive(const String& id) {
			auto* c = findComponentByIdRecursive(id);
			if (LayoutManager->isLayoutComponent(c)) {
				return LayoutManager->isLayoutComponent(c)->getParentLayout();
			}
			return nullptr;
		}

		Component* Layout::addComponentToLayout(Component* componentToAdd) {
			comps.add(componentToAdd);
			addAndMakeVisible(componentToAdd);
			setParent(componentToAdd);
			repaint();
			Layout* l = dynamic_cast<Layout*>(componentToAdd);
			BaseLayoutComponent* bdc = dynamic_cast<BaseLayoutComponent*>(componentToAdd);
			if (l) {
				l->parent = this;
			}

			else if (bdc) {

				bdc->layout = this;

				backNavigationButton.reset(bdc->getBackNavigationButton(this, bdc));

				if (backNavigationButton) {

					backNavigationButton->setAlwaysOnTop(true);
					addAndMakeVisible(*backNavigationButton);
				}
				resized();

			}

			return componentToAdd;
		};

		void Layout::setParent(Component* comp){

			auto* l = LayoutManager->isLayoutComponent(comp);
			auto* bdc = LayoutManager->isBaseLayoutComponent(comp);

			if (l) {
				l->parent = this;
			}
			else if (bdc) {
				bdc->layout = this;
			}
			else {
				jassertfalse;	//comp non � n� un layout n� un BaseLayoutComponent !
			}
		}

		void Layout::paint(Graphics& g) {

			//TODO_UMB
			auto bgC = Colours::red;//ServiceLocator->getResourcesService()->getLookAndFeelColour(mudi_lnf::MudiLookAndFeel::ColourScheme::BG0);
			g.fillAll(bgC);

			if (getItemsSize() == 0) {
				g.fillAll(colourWhenEmpty);
				g.setColour(colourWhenEmpty.contrasting());
				g.drawText(getComponentID(), getLocalBounds(), Justification::centred);
			}

		}

		void Layout::resized() {

			if (backNavigationButton) {

				auto area = getLocalBounds().reduced(margin);

				area = getLocalBounds().withTrimmedLeft(leftMargin)
					.withTrimmedRight(rightMargin)
					.withTrimmedBottom(bottomMargin)
					.withTrimmedTop(topMargin);

				area = getLocalBounds();

				backNavigationButton->setBounds(area.removeFromTop(80).removeFromRight(80).reduced(10));
			}
		}

		void Layout::setCollapsedState(bool shouldBeCollapsed) {
			jassert(isCollapsible.get() == true);
			isCollapsed = shouldBeCollapsed;
			setVisible(!isCollapsed);
			state.setProperty(VTIDs::layoutCollapsedPropertyIdentifier, shouldBeCollapsed, nullptr);
			LayoutManager->layoutCollapsedCallback(id.get(), isCollapsedState());
		}

		bool Layout::isCollapsedState() {
			return isCollapsed;
		}

		const String Layout::getID() {
			return id.get();
		}

		int Layout::getItemsSize() const {
			return comps.size();
		}

		Array<Component*> Layout::getItemsComponent() {
			Array<Component*> temp;
			for (auto* c : comps) {
				temp.add(c);
			}
			return temp;
		}

		Layout* Layout::getParentLayout() {
			return dynamic_cast<Layout*>(parent);
		}

		Array<Layout*> Layout::getChildrenLayout() {
			Array<Layout*> temp;
			for (auto* c : getChildren())
				if (LayoutManager->isLayoutComponent(c))
					temp.add(LayoutManager->isLayoutComponent(c));
			return temp;
		}

		Layout* Layout::getChildLayout(const String& layoutID) {
			for (auto* l : getChildrenLayout())
				if (l->getComponentID().equalsIgnoreCase(layoutID))
					return l;
			return nullptr;
		}

	}
}

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		const Identifier Layout::layoutIdentifier("LAYOUT");
		
		const int Layout::MAX_SLOTS = 100;

		MudiLayoutManager::MudiLayoutManager() : 
			mainLayout(nullptr),
			currentLayoutType("0") 
		{

			state = ValueTree(VTIDs::rootNodeIdentifier);

			buildLayoutList();

		}

		MudiLayoutManager::~MudiLayoutManager() {
			clearLayout();
			resetChangeListener();
			clearSingletonInstance();
		}

		BaseLayoutComponent* MudiLayoutManager::createBaseLayoutComponentFromVt(ValueTree& vt) {

			auto className = vt.getProperty(VTIDs::classNamePropertyIdentifier, "").toString();
			BaseLayoutComponent* bdc = (BaseLayoutComponent*)BaseLayoutComponent::Create(className);

			if (!bdc) {
				bdc = (BaseLayoutComponent*)BaseLayoutComponent::Create("MockLayoutComponent");
				dynamic_cast<MockLayoutComponent*>(bdc)->setText(className);
			}

			bdc->setState(vt);
			bdc->initDawComponent();
			registerComponent(bdc, bdc->getId());
			return bdc;

		}

		MemoryBlock MudiLayoutManager::getIconDataForComponent(const String& componentID) {
			auto* comp = getComponentByID(componentID);
			jassert(comp != nullptr);
			return getIconDataForComponent(comp);
		}

		void MudiLayoutManager::getAllChildrenFromLayoutRecursive(Layout* parent, map<String, Component*>& map) {

			for (Component* child : parent->getChildren()) {

				if (isBaseLayoutComponent(child) || isLayoutComponent(child)) {
					map[child->getComponentID()] = child;
				}

				if (auto* l = isLayoutComponent(child)) {
					getAllChildrenFromLayoutRecursive(l,map);
				}
			
			}

		}

		void MudiLayoutManager::getAllChildrenFromDawLayout(map<String, Component*>& map) {
			if(auto * dawLayout = getDawLayout())
				return getAllChildrenFromLayoutRecursive(dawLayout,map);

			//Assicurati che nell'xml ci siamo il nodo "DawLayout"
			jassertfalse;
		}

		void MudiLayoutManager::getAllChildrenFromDjLayout(map<String, Component*>& map) {
			if (auto* djLayout = getDJLayout())
				return getAllChildrenFromLayoutRecursive(djLayout,map);

			//Assicurati che nell'xml ci siamo il nodo "djLayout"
			jassertfalse;
		}

		void MudiLayoutManager::getAllChildrenFromScreen(int screenNum, map<String, Component*>& map) {
			auto screensVt = getScreensVt();

			if (screensVt.getNumChildren() == 0)
				return;

			if (isPositiveAndBelow(0, screensVt.getNumChildren())) {
				auto screenVt = getScreensVt().getChild(screenNum);
				auto firstLayoutVt = screenVt.getChild(0);
				auto id = firstLayoutVt.getProperty(VTIDs::idPropertyIdentifier, "").toString();
				jassert(id != "");
				if (Layout* l = isLayoutComponent(allComps[id])) {
					return getAllChildrenFromLayoutRecursive(l,map);
				}
			}
			else {
				//Screen number non valido o
				jassertfalse;
			}
			
		}

		MemoryBlock MudiLayoutManager::getIconDataForComponent(Component* comp) {
			if (auto* l = isLayoutComponent(comp))
				return l->iconData;
			
			if (auto* bdw = isBaseLayoutComponent(comp))
				return bdw->getIconData();

			//Il componente deve essere un BaseDaw o un Layout!
			jassertfalse;
			return MemoryBlock();
		}

		void MudiLayoutManager::initChangeListener() {

#if 0
			if (auto* sm = mudi::mudi_api::Api->getCurrentlyFocusedSelectionManagerWithValidEdit()) {

				sm->addChangeListener(this);

			}
#endif // 0

			
		}

		void MudiLayoutManager::resetChangeListener() {

#if 0
			if (auto* sm = mudi::mudi_api::Api->getCurrentlyFocusedSelectionManagerWithValidEdit()) {

				sm->removeChangeListener(this);

			}
#endif // 0


		}

		int MudiLayoutManager::getIntegerLayourVersion(const String& resourceLayoutVersion)
		{
			
			//0.3.9			-> 0 * 1000000 + 3 * 1000 + 9 * 1		= 3000 + 9		= 3009
			//0.3.110		-> 0 * 1000000 + 3 * 1000 + 110 * 1		= 3000 + 110	= 3110
			//0.4.150		-> 0 * 1000000 + 4 * 1000 + 150 * 1     = 4000 + 150	= 4150

			StringArray splittedVersionStringArray;
			splittedVersionStringArray.addTokens(resourceLayoutVersion, ".", "");

			auto majorVersion = splittedVersionStringArray[0].getIntValue();
			auto minorVersion = splittedVersionStringArray[1].getIntValue();
			auto patchVersion = splittedVersionStringArray[2].getIntValue();

			return majorVersion * 1000000 + minorVersion * 1000 + patchVersion;
		}

		ValueTree MudiLayoutManager::getStateFromComponent(Component* c) {
			auto* l = isLayoutComponent(c);
			auto* bdc = isBaseLayoutComponent(c);

			if (l) {
				return l->state;
			}
			if (bdc) {
				return bdc->getPropertiesState();
			}

			return ValueTree();
		}

		const String MudiLayoutManager::getCurrentLayoutType() {

			return currentLayoutType.equalsIgnoreCase("0") ? "" : currentLayoutType;

		}

		Component* MudiLayoutManager::buildLayout(const File& appPrefs, const char* resource, int file_size, const String& xmlFile, const String& layoutType) {

			jassert(!layoutType.contains("abcdefghilmnopqrstuvz"));
			
			currentLayoutType = layoutType.equalsIgnoreCase("0") ? "" : layoutType;

			String xmlLoadFile = "layout" + currentLayoutType + ".xml";

			//reading xml from resource
			MemoryInputStream ms((const void*)resource, file_size, false);

			String xmlData = ms.readEntireStreamAsString();

			XmlDocument resourcelayoutDocument(xmlData);

			const std::unique_ptr<XmlElement> resourceLayout = resourcelayoutDocument.getDocumentElement();

			//The version has a format kind like "1.2.3", so we need to split it and sum the value
			String resourceLayoutVersion = resourceLayout->getStringAttribute(VTIDs::layoutVersionPropertyIdentifier, "0.0.0");

			int version = getIntegerLayourVersion(resourceLayoutVersion);
			
			const String resourceLayoutName = resourceLayout->getStringAttribute(VTIDs::layoutNamePropertyIdentifier, "");
			
			bool loadedFromCache = false;

			if (xmlFile != "")
				xmlLoadFile = xmlFile;

			//loading layout from user
			xmlLayout = appPrefs.getChildFile(xmlLoadFile);
			
			//TODO per ora lo leggo sempre da hdd 
			//Controllo se il layout � gi� caricato in cache
			if (xmlLayout.exists())
			{

				String cacheResourceLayoutVersion = XmlDocument(xmlLayout).getDocumentElement()->getStringAttribute(VTIDs::layoutVersionPropertyIdentifier, "0.0.0");

				const auto currentAppVersion = getIntegerLayourVersion(cacheResourceLayoutVersion);

				if (version < currentAppVersion) {
					//The Layout version is < the the current app version. Remeber to change the layout version in the .xml file under resources\layout folder !!
					jassertfalse;
				}

				loadedFromCache = loadedFromFile(xmlLayout);

				if (version > currentLayoutVersion) {
					xmlLayout.deleteFile();
					createLayoutFromResource(resource, file_size, xmlLayout);
				}
			}
			else {

				createLayoutFromResource(resource, file_size, xmlLayout);

			}


#if 0
			//Se � gi� in cache ed il numero di versione coicide lo prendo
			if (loadedFromCache && resourceLayoutVersion >= currentLayoutVersion) {

				xmlLayout.deleteFile();
				xmlLayout.create();
				//update layout
				xmlLayout.appendText(xmlData);

			}
			else {	//Altrimenti lo ricreo dalle BinaryRes
				createLayoutFromResource(resource, file_size, xmlLayout);
			}
#endif // 0

			jassert(state.isValid());

			if (auto* ml = createLayout()) {

				return ml;

			}

			//[ERROR] Main layout not created correctly.
			jassertfalse;
			return nullptr;
		}
		
		Component* MudiLayoutManager::buildLayout(const String layoutType) {
			
#if 0
			int file_size = 0;

			String currentLayoutType = layoutType.equalsIgnoreCase("0") ? "" : layoutType;

			String layoutRes = "layout" + currentLayoutType + "_xml";

			//TODO_UMB
			const char* xmlData = BinaryData::getNamedResource(layoutRes.toRawUTF8(), file_size);

			File appPrefs = Api->getEngine()->getPropertyStorage().getAppPrefsFolder();

			return buildLayout(appPrefs, xmlData, file_size, "", currentLayoutType);
#endif // 0

			return nullptr;

		}

		void MudiLayoutManager::buildMiniatureLayout(StretchLayout* stretchParent, Rectangle<int>& area , std::map<String, Rectangle<int>>& map) {
					

			for (auto* child : stretchParent->getChildrenLayout()) {

				int ms = child->miniatureSlots;

				//Se � l'ultimo figlio gli do tutta l'area rimasta!
				bool isLast = stretchParent->getChildrenLayout().getLast() == child;

				float proportion = (float)(ms) / (float)(Layout::MAX_SLOTS);

				auto currentArea = Rectangle<int>();

				if (stretchParent->isVertical()) {
					auto h = area.getHeight();
					currentArea = isLast ? area : area.removeFromTop(jmax(22, (int)(proportion * h)));
				}
				else {
					auto w = area.getWidth();
					currentArea = isLast ? area : area.removeFromLeft(jmax(22, (int)(proportion * w)));
				}

				map[child->id] = currentArea;

				//Solo per i figli Stretch o StretchMultiView applico la ricorsione
				if (auto* stretchChild = LayoutManager->isStretchLayoutComponent(child)) {
					buildMiniatureLayout(stretchChild, currentArea,map);
				}
				
			}


		}

		void MudiLayoutManager::buildMiniatureLayoutForDaw(Rectangle<int>& area, std::map<String, Rectangle<int>>& map) {
			if (StretchLayout* dawLayout = getDawLayout())
				return buildMiniatureLayout(dawLayout,area,map);

			//DawLayout non presente nell'xml oppure non � di tipo "StretchLayout"!
			jassertfalse;
		}

		void MudiLayoutManager::buildMiniatureLayoutForDj(Rectangle<int>& area, std::map<String, Rectangle<int>>& map) {
			if (StretchLayout* djLayout = getDawLayout())
				return buildMiniatureLayout(djLayout, area, map);

			//djLayout non presente nell'xml oppure non � di tipo "StretchLayout"!
			jassertfalse;
		}

		void MudiLayoutManager::buildMiniatureLayoutForScreen(int screenNum, Rectangle<int>& area, std::map<String, Rectangle<int>>& map) {
			auto screensVt = getScreensVt();

			if (screensVt.getNumChildren() == 0)
				return;

			if (isPositiveAndBelow(0, screensVt.getNumChildren())) {
				auto screenVt = getScreensVt().getChild(screenNum);
				auto firstLayoutVt = screenVt.getChild(0);
				auto id = firstLayoutVt.getProperty(VTIDs::idPropertyIdentifier, "").toString();
				jassert(id != "");
				if (StretchLayout* sl = isStretchLayoutComponent(allComps[id])) {
					return buildMiniatureLayout(sl, area ,map);
				}

				//Se sei qui � perch� il primo Layout sotto il nodo "SCREEN" in questione non � di tipo Stretch!
				jassertfalse;
				
			}
			else {
				//Screen number non valido
				jassertfalse;
			}
		}

		bool MudiLayoutManager::loadedFromFile(const File& xmlFile) {


			XmlDocument layoutDocument(xmlFile);

			const std::unique_ptr<XmlElement> xmlEl = layoutDocument.getDocumentElement();

			if (xmlEl) {

				auto layoutVesion = xmlEl->getStringAttribute(VTIDs::layoutVersionPropertyIdentifier, "0.0.0");

				currentLayoutVersion = getIntegerLayourVersion(layoutVesion);
				
				state = ValueTree::fromXml(*xmlEl);

				if (state.isValid())
				{
					//DBG("DawLayoutManager::loadFromFile | loaded layout : " + xmlFile.getFullPathName()+ " " + state.toXmlString());
					return true;
				}

			}

			return false;
		}

		void MudiLayoutManager::saveLayout() {
			if (xmlLayout.exists()) {
				xmlLayout.replaceWithText(state.toXmlString());
			}
		}

		void resetLayout() {



		}

		void MudiLayoutManager::createLayoutFromResource(const char* resource, int file_size, const File& layoutFile) {
			MemoryInputStream ms((const void*)resource, file_size, false);
			String xmlData = ms.readEntireStreamAsString();
			XmlDocument layoutDocument(xmlData);

			const std::unique_ptr<XmlElement> xml = layoutDocument.getDocumentElement();

			//TODO_UMB 
			auto currentAppVersion = "1.0.0";//Api->getVersionStringNumber();

			state = ValueTree::fromXml(*xml);

			//DBG("DawLayoutManager::loadFromFile | created layout : " + state.toXmlString());

			//saving layout file
			layoutFile.replaceWithText(xmlData);
			//layoutFile.appendText(xmlData);
		}

		void MudiLayoutManager::createMainLayout() {
			auto mainLayoutNode = state.getChild(0);
			if (!mainLayout) {
				auto layoutType = mainLayoutNode.getProperty(VTIDs::layoutModePropertyIdentifier).toString();
				if (layoutType.equalsIgnoreCase(VTIDs::stretchLayoutPropertyIdentifier.toString())) {
					mainLayout = createStretchLayoutFromVt(mainLayoutNode);
				}
				else if(layoutType.equalsIgnoreCase(VTIDs::stackLayoutPropertyIdentifier.toString())) {
					mainLayout = createStackLayoutFromVt(mainLayoutNode);
				}
				else if (layoutType.equalsIgnoreCase(VTIDs::stretchMultiViewLayoutPropertyIdentifier.toString())) {
					mainLayout = createStretchMultiViewLayoutFromVt(mainLayoutNode);
				}
				else {
					jassertfalse;	//Nessun layoutMode valido.
				}
			}
		}

		Component* MudiLayoutManager::createLayout() {
			
			//INFO : se crei un component/layout e non gli fai l'addComponent verso un layout avrai dei leak :)
			//INFO : Devono essere presenti i nodi "DawLayout" e "DjLayout" che indicano rispettivamente le root per la daw e per la dj view !

			//jassert(owner != nullptr);	//Se l'assert fallisce hai dimenticato di chiamare il setOwnerComponent !
			jassert(state.isValid());

			DBG(state.toXmlString());

			createMainLayout();

			//auto& eventsVt = state.getChildWithName(VTIDs::eventsNodeIdentifier);
			auto eventsVt = getEventsVt();
			eventProcessor.setState(eventsVt);
			for (auto child : eventsVt) {
				createEvent(child);
			}

			
			for (auto child : mainLayout->state) {
				buildLayoutRecursive(mainLayout, child);
			}

			//auto& screensVt = state.getChildWithName(VTIDs::screensNodeIdentifier);
			auto screensVt = getScreensVt();

			if (screensVt.isValid()) {
				buildScreens(screensVt);
			}

			checkForCollapsedLayout();
			
			layoutCreatedCallback(xmlLayout.getFileName());

			return mainLayout;
			
		}

		ValueTree MudiLayoutManager::getEventsVt() {

			//Assicurati che lo state sia stato caricato
			jassert(state.isValid());

			auto eventsVt = state.getChildWithName(VTIDs::eventsNodeIdentifier);

			//Assicurati di aver registrato il nodo <EVENTS> ... </EVENTS> nell'xml
			jassert(eventsVt.isValid());

			return eventsVt;
		}

		ValueTree MudiLayoutManager::getScreensVt() {
			//Assicurati che lo state sia stato caricato
			jassert(state.isValid());

			auto screensVt = state.getChildWithName(VTIDs::screensNodeIdentifier);

			//Assicurati di aver registrato il nodo <SCREENS> ... </SCREENS> nell'xml
			jassert(screensVt.isValid());

			return screensVt;
		}

		void MudiLayoutManager::buildLayoutRecursive(Layout* parent, ValueTree& child) {

			
			if (child.hasType(VTIDs::componentNodeIdentifier)) {

				auto* c = createBaseLayoutComponentFromVt(child);

				parent->addComponent(c);

				DBG("ParentLayout : [" + parent->getComponentID() + "] adding COMPONENT [" + c->getComponentID() +"]");

			}
			else if (child.hasType(VTIDs::layoutNodeIdentifier)) {

				Layout* l = createLayoutFromVt(child);

				parent->addComponent(l);

				DBG("ParentLayout : [" + parent->getComponentID() + "] adding LAYOUT [" + l->getComponentID()+"]");

				for (auto subChild : child) {
					buildLayoutRecursive(l, subChild);
				}

			}

		}

		void MudiLayoutManager::buildScreens(ValueTree& screensVt) {

			for (auto screen : screensVt) {

				//FIRST NODE OF SCREEN MUST BE A LAYOUT
				auto firstLayoutNode = screen.getChild(0);
				auto* l = createLayoutFromVt(firstLayoutNode);
				if (l) {
					for (auto child : firstLayoutNode)
						buildLayoutRecursive(l, child);
				}
				else {
					jassertfalse;	//il primo nodo di un nodo SCREEN non � un LAYOUT !
				}
				DockManager->createWindowFromVtAndAddContent(screen,l);
			}
		}

		StretchLayout* MudiLayoutManager::createStretchLayoutFromVt(ValueTree& state) {
			auto* stretchLayout = new StretchLayout(state);
			registerComponent(stretchLayout, stretchLayout->getID());
			return stretchLayout;
		}

		StackLayout* MudiLayoutManager::createStackLayoutFromVt(ValueTree& state) {
			auto* stackLayout = new StackLayout(state);
			registerComponent(stackLayout, stackLayout->getID());

			return stackLayout;
		}

		StretchMultiViewLayout* MudiLayoutManager::createStretchMultiViewLayoutFromVt(ValueTree& state) {
			auto* stretchMultiViewLayout = new StretchMultiViewLayout(state);
			registerComponent(stretchMultiViewLayout, stretchMultiViewLayout->getID());

			return stretchMultiViewLayout;
		}

		Layout* MudiLayoutManager::createLayoutFromVt(ValueTree& vt) {

			auto id = vt.getProperty(VTIDs::idPropertyIdentifier).toString();
			auto layoutMode = vt.getProperty(VTIDs::layoutModePropertyIdentifier).toString();

			jassert(layoutMode != "");	//E' obbligatorio definire la propriet� "layoutMode" di un Layout nell'xml

			Layout* layout = nullptr;

			if (layoutMode == VTIDs::stretchLayoutPropertyIdentifier.toString()) {
				
				layout = createStretchLayoutFromVt(vt);
				

			}
			else if(layoutMode == VTIDs::stackLayoutPropertyIdentifier.toString()) {

				layout = createStackLayoutFromVt(vt);
				
			}
			else if (layoutMode == VTIDs::stretchMultiViewLayoutPropertyIdentifier.toString()) {

				layout = createStretchMultiViewLayoutFromVt(vt);
				
			}
			else {

				jassertfalse;	//No valid layoutMode recognized!

			}

			jassert(layout != nullptr);

			return layout;

		}

		Array<double> MudiLayoutManager::getSizesForComponent(ValueTree& vt) {
			Array<double> sizes;

			double prefSize = vt.getProperty(VTIDs::defPreferredSizePropertyIdentifier, 0.0);
			double minSize = vt.getProperty(VTIDs::defminimumSizePropertyIdentifier, 0.0);
			double maxSize = vt.getProperty(VTIDs::defmaximumSizePropertyIdentifier, 0.0);

			sizes.add(minSize);
			sizes.add(maxSize);
			sizes.add(prefSize);

			return sizes;
		}

		Array<double> MudiLayoutManager::getSizesForComponent(Component* c) {
			Array<double> sizes;

			auto* bdc = isBaseLayoutComponent(c);
			auto* l = isLayoutComponent(c);

			if (bdc) {
				sizes.add(bdc->minSize);
				sizes.add(bdc->maxSize);
				sizes.add(bdc->prefSize);
			}
			else if (l) {
				sizes.add(l->minSize);
				sizes.add(l->maxSize);
				sizes.add(l->prefSize);
			}
			else {
				jassertfalse;
			}

			return sizes;

		}

		void MudiLayoutManager::setPreferredSizeForComponent(Component* c, double newPrefSize) {
			auto* bdc = isBaseLayoutComponent(c);
			auto* l = isLayoutComponent(c);

			if (bdc) {
				bdc->prefSize = newPrefSize;
			}
			else if (l) {
				l->prefSize = newPrefSize;
			}
			else {
				jassertfalse;
			}
		}

		void MudiLayoutManager::setMinSizeForComponent(Component* c, double newMinSize) {
			auto* bdc = isBaseLayoutComponent(c);
			auto* l = isLayoutComponent(c);

			if (bdc) {
				bdc->prefSize = newMinSize;
			}
			else if (l) {
				l->prefSize = newMinSize;
			}
			else {
				jassertfalse;
			}
		}

		void MudiLayoutManager::setMaxSizeForComponent(Component* c, double newMaxSize) {
			auto* bdc = isBaseLayoutComponent(c);
			auto* l = isLayoutComponent(c);

			if (bdc) {
				bdc->prefSize = newMaxSize;
			}
			else if (l) {
				l->prefSize = newMaxSize;
			}
			else {
				jassertfalse;
			}
		}

		void MudiLayoutManager::clearLayout() {

			allComps.clear();

			DockManager->closeWindows();

			mainLayout = nullptr;
			
		}

		BaseLayoutComponent* MudiLayoutManager::isBaseLayoutComponent(Component* componentToCheck) {
			return dynamic_cast<BaseLayoutComponent*>(componentToCheck);
		}

		Layout* MudiLayoutManager::isLayoutComponent(Component* componentToCheck) {
			return dynamic_cast<Layout*>(componentToCheck);
		}

		StackLayout* MudiLayoutManager::isStackLayoutComponent(Component* componentToCheck) {
			return dynamic_cast<StackLayout*>(componentToCheck);
		}

		StretchLayout* MudiLayoutManager::isStretchLayoutComponent(Component* componentToCheck) {
			return dynamic_cast<StretchLayout*>(componentToCheck);
		}

		StretchMultiViewLayout* MudiLayoutManager::isStretchMultiViewLayoutComponent(Component* componentToCheck) {
			return dynamic_cast<StretchMultiViewLayout*>(componentToCheck);
		}

		bool MudiLayoutManager::isTopComponent(const String& id) {

			if (auto compToCheck = allComps[id]) {

				auto stackLayout = isLayoutComponent(compToCheck);

				if (!stackLayout) {

					if (auto baseComp = isBaseLayoutComponent(compToCheck)) {

						stackLayout = baseComp->layout;

					}

				}
				
				if (stackLayout)
					return stackLayout->getTopLevelComponent()->getComponentID() == id;


			}

			jassertfalse;
			return false;

		}

		void MudiLayoutManager::setTopComponent(const String& id, bool shoudBeVisible) {

			if (auto compToMaKeVisible = allComps[id]) {

				if (auto bdc = isBaseLayoutComponent(compToMaKeVisible)) {

					if(auto sl = dynamic_cast<StackLayout*>(bdc->layout)){
					
						sl->setTopComponent(compToMaKeVisible);
						return;

					}

				}

				if (auto layout = isLayoutComponent(compToMaKeVisible)) {

					if(auto sl = dynamic_cast<StackLayout*>(layout->getParentLayout())){
					
						sl->setTopComponent(compToMaKeVisible);
						return;

					}

				}

			}
			
			//Il componente deve essere ( o deve essere figlio di ) uno stackLayout
			jassertfalse;	//l'id del componente non � registrato
			
		}

		void MudiLayoutManager::collapseLayout(const String& id, bool shoudBeCollapsed, const String& idToSwitch) {

			auto* comp = allComps[id];

			if (comp) {

				auto* l = isLayoutComponent(comp);

				if (l) {

					auto* parent = l->getParentLayout();

					if (parent) {

						l->setCollapsedState(shoudBeCollapsed);

						auto* sl = isStretchLayoutComponent(parent);

						if (sl && sl->dividerBar) {
							sl->dividerBar->setVisible((!shoudBeCollapsed));
						}

						bool isAtLeastOneChildLayoutVisibile = false;
						int i = 0;
						auto children = parent->getChildrenLayout();
						while (!isAtLeastOneChildLayoutVisibile && i < children.size()) {
							isAtLeastOneChildLayoutVisibile = children[i]->isVisible();
							i++;
						}

						if (parent->isCollapsible) {

							if (!isAtLeastOneChildLayoutVisibile) {
								collapseLayout(parent->getComponentID(), true);
							}
							else if (parent->isCollapsedState()) {
								collapseLayout(parent->getComponentID(), false);
							}

						}
						else if (shoudBeCollapsed && !isAtLeastOneChildLayoutVisibile) {

							collapseLayout(idToSwitch.isNotEmpty() ? idToSwitch : l->getComponentID(), false);;

						}
						
						parent->resized();

					}
					else {
						jassertfalse;	//Stai provando a collassare/espandere il topLevel Layout... non � una buona idea
					}
					
					return;
				}
			}
			jassertfalse;	//l'id del componente non � registrato

		}

		bool MudiLayoutManager::isLayoutCollapsed(const String& id) {

			auto* comp = allComps[id];

			if (comp) {

				if(auto* l = isLayoutComponent(comp))

					return l->isCollapsedState();

			}

			
			return false;

		}

		Component* MudiLayoutManager::getComponentByID(const String& id) {
			return allComps[id];
		}

		map<String, Component*> MudiLayoutManager::getAllComps() {
			return allComps;
		}

		MudiEventProcessor& MudiLayoutManager::getEventProcessor() {
			return eventProcessor;
		}

		void MudiLayoutManager::createEvent(ValueTree& eventVt) {
			if (eventVt.hasType(VTIDs::userEventPropertyIdentifier)) {
				String id = eventVt.getProperty(VTIDs::idPropertyIdentifier, "").toString();

				jassert(id != "");	//L'id di un evento non pu� essere nullo !

				MudiLayoutUserEvent* userEvent = new MudiLayoutUserEvent(id,"");

				for (int j = 0; j < eventVt.getNumChildren(); j++)
				{
					ValueTree subevtVt = eventVt.getChild(j);
					initEvent(subevtVt, userEvent);
				}

				eventProcessor.addEvent(userEvent, id);

			}
			else if (eventVt.hasType(VTIDs::selectedEventPropertyIdentifier)) {
				initEvent(eventVt,nullptr);
			}

		}

		void MudiLayoutManager::initEvent(ValueTree& vt, MudiLayoutUserEvent* userEvent) {


			if (vt.hasType(VTIDs::selectedEventPropertyIdentifier))
			{
				String className = vt.getProperty(VTIDs::classNamePropertyIdentifier, "").toString();
				String target = vt.getProperty(VTIDs::targetPropertyIdentifier, "").toString();
				String action = vt.getProperty(VTIDs::actionPropertyIdentifier, "").toString();
				String id = vt.getProperty(VTIDs::idPropertyIdentifier, "").toString();

				jassert(id != "");	//ID dell'evento non registrato nell'xml !

				JamLayoutSelectedEvent* ev = new JamLayoutSelectedEvent(id,className, target, action);
				ev->state = vt;
				//ev->id = vt.getProperty(VTIDs::idPropertyIdentifier, "");

				if (userEvent)
					//userEvent->addEvent(ev, VTIDs::selectedEventPropertyIdentifier.toString());
					userEvent->addEvent(ev, id);
				else
					//eventProcessor.addEvent(ev, vt.getType().toString());
					eventProcessor.addEvent(ev, id);

			}
		}

		void MudiLayoutManager::fireUserEvent(const String& eventId) {
			eventProcessor.fireEvent(VTIDs::userEventPropertyIdentifier.toString(), "", eventId);
		}

		void MudiLayoutManager::changeListenerCallback(ChangeBroadcaster* source) {

			//TODO_UMB questo riguarda la gestione degli eventi
#if 0
			auto selectionManager = dynamic_cast<SelectionManager*>(source);

			if (selectionManager)
			{
				Selectable* sel = selectionManager->getSelectedObject(0);
				bool isSel = selectionManager->isSelected(sel);


				WaveAudioClip* selectedObject = dynamic_cast<WaveAudioClip*>(sel);

				if (selectedObject)
				{
					if (isSel)
						eventProcessor.fireEvent(VTIDs::selectedEventPropertyIdentifier.toString(), "WaveAudioClip", "show_clip_ditor_selected2");
					else
						eventProcessor.fireEvent(VTIDs::unselectedEventPropertyIdentifier.toString(), "WaveAudioClip", "show_clip_ditor_selected2");

					return;
				}

				MidiClip* midiClipObject = dynamic_cast<MidiClip*>(sel);

				if (midiClipObject)
				{
					if (isSel)
						eventProcessor.fireEvent(VTIDs::selectedEventPropertyIdentifier.toString(), "MidiClip", "show_clip_editor_selected");
					else
						eventProcessor.fireEvent(VTIDs::unselectedEventPropertyIdentifier.toString(), "MidiClip", "show_clip_editor_selected");

					return;
				}

				ClipTrack* clipTrack = dynamic_cast<ClipTrack*>(sel);

				/*if (clipTrack)
				{
					if (isSel)
						eventProcessor.fireEvent(VTIDs::selectedEventPropertyIdentifier.toString(), "ClipTrack","show_inspector_selected");
					else
						eventProcessor.fireEvent(VTIDs::unselectedEventPropertyIdentifier.toString(), "ClipTrack", "show_inspector_selected");

					return;
				}*/
			}
#endif // 0
		}

		void MudiLayoutManager::checkForCollapsedLayout() {
			for (auto& [k, v] : allComps) {
				auto* l = isLayoutComponent(v);
				if (l && l->isCollapsedState()) {
					LayoutManager->collapseLayout(l->getComponentID(), true);
				}
			}
		}

		void MudiLayoutManager::checkForDawAndDjLayout() {

		}

		void MudiLayoutManager::registerComponent(Component* compToRegister, String compId) {
			
			if (auto* comp = allComps[compId]) {
				//il compId passato � gi� stato assegnato ad un atro componente !!
				//Controllare nell'xml che TUTTI i nodi abbiano id diversi !
				jassertfalse;
			}

			allComps[compId] = compToRegister;
		}

		StretchLayout* MudiLayoutManager::getDawLayout() {
			if(auto * dawLayout = isStretchLayoutComponent(getComponentByID("DawLayout")))
				return dawLayout;

			//Controlla nell'xml che ci sia il nodo "DawLayout"
			jassertfalse;
			return nullptr;
		}

		StretchLayout* MudiLayoutManager::getDJLayout() {
			if (auto* djLayout = isStretchLayoutComponent(getComponentByID("DjLayout")))
				return djLayout;
			
			//Controlla nell'xml che ci sia il nodo "DawLayout"
			jassertfalse;
			return nullptr;

		}

		void MudiLayoutManager::layoutCreatedCallback(const String& layoutID) {

			layoutListeners.call([layoutID, this](MudiLayoutListener& l) { l.layoutCreated(layoutID); });

		}

		void MudiLayoutManager::layoutChangedCallback(const String& oldLayoutID, const String& newLayoutID) {

			layoutListeners.call([oldLayoutID, newLayoutID, this](MudiLayoutListener& l) { l.layoutChanged(oldLayoutID,newLayoutID); });

		}

		void MudiLayoutManager::layoutCollapsedCallback(const String& layoutID, bool isCollpased)
		{
		}

		const std::map<String, String> MudiLayoutManager::getLayoutFileNameMap() {

			return layoutFileNameMap;

		}

		
		StringArray MudiLayoutManager::getAvailableRawLayoutList() {

			StringArray layoutFilesArray;

			//TODO_UMB
#if 0
			for (const String& l : ServiceLocator->getResourcesService()->getNamedResourceList()) {

				if (l.containsIgnoreCase("layout")) {

					layoutFilesArray.add(l);

				}

			}
#endif // 0


			return layoutFilesArray;

		}

		void MudiLayoutManager::buildLayoutList() {

			for (auto l : getAvailableRawLayoutList()) {

				int layoutFileSize = 0;

				//TODO_UMB
				const char* resource = "";//ServiceLocator->getResourcesService()->getNamedResource(l.toRawUTF8(), layoutFileSize);

				MemoryInputStream ms((const void*)resource, layoutFileSize, false);

				String xmlData = ms.readEntireStreamAsString();

				//DBG(xmlData.toStdString());

				XmlDocument resourcelayoutDocument(xmlData);

				const std::unique_ptr<XmlElement> resourceLayout = resourcelayoutDocument.getDocumentElement();

				const String resourceLayoutName = resourceLayout->getStringAttribute(VTIDs::layoutNamePropertyIdentifier, "");

				//TODO_UMB
				const String originalFileName = "";//ServiceLocator->getResourcesService()->getNamedResourceOriginalFilename(l.toRawUTF8());

				layoutFileNameMap[originalFileName] = resourceLayoutName;

			}

		}

		JUCE_IMPLEMENT_SINGLETON(MudiLayoutManager);

	}

}


namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		MudiEventProcessor::MudiEventProcessor() {}

		MudiEventProcessor::~MudiEventProcessor() {
			resetProcessor();
		}

		void MudiEventProcessor::resetProcessor() {
			eventsMap.clear();
		}

		void MudiEventProcessor::addEvent(MudiLayoutEvent* e, const String& eventID) {
			eventsMap.insert(pair<String, MudiLayoutEvent*>(eventID, e));
			events.add(e);
		}

		void MudiEventProcessor::fireEvent(const String& eventType, const String& className, const String& eventId) {

			auto* e = eventsMap[eventId];
			if (e) {

				auto* userEvent = dynamic_cast<MudiLayoutUserEvent*>(e);
				auto* selEvent = dynamic_cast<JamLayoutSelectedEvent*>(e);

				if (userEvent) {
					for (auto* subEvent : userEvent->subEvents)
						fireEventNotifyingListener(e);
				}
				else if (selEvent) {
					fireEventNotifyingListener(selEvent);
				}
				else {
					jassertfalse;	//Tipo di evento errato!
				}

			}
			else {
				//jassertfalse; //l'id dell'evento non � registrato!
			}

		}

		void MudiEventProcessor::setState(ValueTree newState) {

			eventsMap.clear();
			events.clear();

			state = newState;

		}

		void MudiEventProcessor::fireEventNotifyingListener(MudiLayoutEvent* e) {

			LayoutManager->sendPreTrigger(e);

			e->trigger();

			LayoutManager->sendPostTrigger(e);
		}

		void MudiEventProcessor::fireSelectedEvent(const String& className, const String& eventID) {

		}

		void MudiEventProcessor::fireUserEvent(const String& className, const String& eventID) {

		}

	}


	namespace mudi_layout
	{
		using namespace juce;

		MudiLayoutEvent::MudiLayoutEvent(const String& id, const String& className) : id(id),className(className) {

		}

        MudiLayoutEvent::~MudiLayoutEvent(){}
        
		void MudiLayoutEvent::addEvent(MudiLayoutEvent* e, const String& eventCat){}

		// ======= MudiLayoutUserEvent ============ //

		MudiLayoutUserEvent::MudiLayoutUserEvent(const String& id, const String& className) : MudiLayoutEvent(id,className) {
			//id = className;
		}

		MudiLayoutUserEvent::~MudiLayoutUserEvent() {
			subEvents.clear();
		}

		bool MudiLayoutUserEvent::trigger() {
			for(auto * e : subEvents)
				LayoutManager->getEventProcessor().fireEvent(e->getType(), e->className, e->id);
			return true;
		}

		String MudiLayoutUserEvent::getType() {
			return VTIDs::userEventPropertyIdentifier.toString();
		}

		void MudiLayoutUserEvent::addEvent(MudiLayoutEvent* e, const String& eventID) {
			LayoutManager->getEventProcessor().addEvent(e, eventID);
			subEvents.add(e);
		}

		// ========= JamLayoutSelectedEvent =========== //

		JamLayoutSelectedEvent::JamLayoutSelectedEvent(const String& id, const String& className, const String& target, const String& action)
			: MudiLayoutEvent(id,className),target(target),action(action)
		{

		}

		JamLayoutSelectedEvent::~JamLayoutSelectedEvent() {

		}

		bool JamLayoutSelectedEvent::trigger() {

			if (action == "show") {
				LayoutManager->setTopComponent(target, true);
				return true;
			}

			else if (action == "collapse") {
				LayoutManager->collapseLayout(target, true);
				return true;
			}

			else if (action == "expand") {
				LayoutManager->collapseLayout(target, false);
				return true;
			}

			jassertfalse; //Nessuna azione valida riconosciuta
			return false;

		}

		String JamLayoutSelectedEvent::getType() {
			return VTIDs::selectedEventPropertyIdentifier.toString();

		}

	}

}



