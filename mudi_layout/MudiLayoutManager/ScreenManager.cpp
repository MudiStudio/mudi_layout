namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		MudiSurfaceScreen::AttacchedMonitor::AttacchedMonitor(MudiSurfaceScreen* surfaceOwner, int monitorIDx) :
			monitorIDx(monitorIDx),
			surfaceOwner(surfaceOwner)
		{

			if (auto d = getAttacchedDisplay())
				coveredArea = d->userArea;

			jassert(!coveredArea.isEmpty() && coveredArea.isFinite());

		}

		MudiSurfaceScreen::AttacchedMonitor::AttacchedMonitor(MudiSurfaceScreen* surfaceOwner, int monitorIDx, juce::Rectangle<int> coveredArea) :
			monitorIDx(monitorIDx),
			coveredArea(coveredArea),
			surfaceOwner(surfaceOwner)
		{

			jassert(!coveredArea.isEmpty() && coveredArea.isFinite());

		}

		const Displays::Display* MudiSurfaceScreen::AttacchedMonitor::getAttacchedDisplay() {

			return ScreenManager->getDisplay(monitorIDx);

		}

		void MudiSurfaceScreen::AttacchedMonitor::setCoveredArea(Rectangle<int> newCoveredArea) {

			coveredArea = newCoveredArea;
			
			auto amVt = surfaceOwner->state.getChildWithProperty(IDs::index, monitorIDx);

			if (amVt.isValid()) {

				amVt.setProperty("area", newCoveredArea.toString(), nullptr);

			}

		}

		// --- //

		MudiSurfaceScreen::MudiSurfaceScreen(ValueTree& state) : 
			state(state) ,
			id((int)state.getProperty(IDs::id)),
			name(state.getProperty(IDs::name).toString())
		{

			for (auto amVt : state) {

				auto displayIndex = (int)amVt.getProperty(IDs::index);
				auto coveredArea = Rectangle<int>::fromString(amVt.getProperty("area").toString());

				AttacchedMonitor* am = new AttacchedMonitor(this, displayIndex, coveredArea);
				attacchedMonitors.add(am);

			}

		}

		MudiSurfaceScreen::~MudiSurfaceScreen() {
			notifyListenersOfDeletion();
		}

		juce::String MudiSurfaceScreen::getSelectableDescription() {

			return "SurfaceScreen_" + id;

		}

		MudiSurfaceScreen::AttacchedMonitor* MudiSurfaceScreen::getAttacchedDisplay(int idx) {

			return attacchedMonitors[idx];

		}


#if 0
		Array<const Displays::Display*> MudiSurfaceScreen::getAttacchedDisplays() {

			Array<const Displays::Display*> displays;

			for (auto am : attacchedMonitors)
				displays.add(am->getAttacchedDisplay());

			return displays;

		}

		const Displays::Display* MudiSurfaceScreen::getAttacchedDisplay(int idx) {

			if (auto am = attacchedMonitors[idx])
				return am->getAttacchedDisplay();

			return nullptr;

		}
#endif // 0


		void MudiSurfaceScreen::attacchDisplay(const Displays::Display* d, juce::Rectangle<int> coveredArea) {

			auto idx = ScreenManager->getDisplayIndex(d);

			if (idx == -1) {

				idx = 1;

			}

			attacchDisplay(idx, coveredArea);


		}
		
		void MudiSurfaceScreen::attacchDisplay(int displayIndex, juce::Rectangle<int> coveredArea) {

			if(hasAttacchedScreen(displayIndex)) return;

			AttacchedMonitor* am = new AttacchedMonitor(this,displayIndex, coveredArea);
			attacchedMonitors.add(am);

			auto vt = ValueTree("ATTACHED_MONITOR");

			vt.setProperty(IDs::index, displayIndex, nullptr);
			vt.setProperty("area", coveredArea.toString(), nullptr);

			state.appendChild(vt, nullptr);

			ScreenManager->save();

		}

		bool MudiSurfaceScreen::hasAttacchedScreen(int displayIndex) {

			return state.getChildWithProperty(IDs::index, displayIndex).isValid();

		}

		void MudiSurfaceScreen::detachDisplay(int displayIndex) {

			for (auto am : attacchedMonitors) {

				if (am->monitorIDx == displayIndex) {

					state.removeChild(state.getChildWithProperty(IDs::index, displayIndex), nullptr);

					attacchedMonitors.removeObject(am);

					return;

				}

			}

		}

		// ---------------------------- //

		MudiScreenManager::MudiScreenManager() :
			e(*Api->getEngine()),
			surfaceScreenSelectionManager(new SelectionManager(*Api->getEngine()))
		{

			if (!e.getPropertyStorage().getXmlProperty(SettingID::screenSurfaces)) {

				e.getPropertyStorage().setXmlProperty(SettingID::screenSurfaces, XmlElement("SCREEN_SURFACES"));

			}

			surfaceScreensXML = e.getPropertyStorage().getXmlProperty(SettingID::screenSurfaces);

			state = ValueTree("SCREEN_SURFACES");

			state.addListener(this);

			//loadSurfaceScreen();
			load();

		}

		MudiScreenManager::~MudiScreenManager() {

			state.removeListener(this);

			clearSingletonInstance();

		}

		void MudiScreenManager::load() {

			if (surfaceScreensXML) {

				auto screenSurfacesVt = ValueTree::fromXml(*surfaceScreensXML.get());

				if (screenSurfacesVt.isValid()) {

					for (auto screenSurfaceVt : screenSurfacesVt) {

						state.appendChild(screenSurfaceVt.createCopy(), nullptr);

					}

				}

			}

		}

		void MudiScreenManager::save() {

			surfaceScreensXML = XmlDocument::parse(state.toXmlString());
			e.getPropertyStorage().setXmlProperty(SettingID::screenSurfaces, *surfaceScreensXML);

		}

		MudiSurfaceScreen* MudiScreenManager::getSurfaceScreenFromVt(const ValueTree& vt) {

			int id = vt.getProperty(IDs::id);

			return getSurfaceScreenById(id);

		}

		MudiSurfaceScreen* MudiScreenManager::getSurfaceScreenById(const int& id) {

			return surfaceScreensMap[id].get();

#if 0
			for (auto ss : surfaceScreens)
				if (ss->id == id)
					return ss;

			return nullptr;
#endif // 0


		}

		ValueTree MudiScreenManager::createSurfaceScreenVt(const int& id, String& name) {

			if (name.isEmpty())
				name = generateName();

			auto ssVt = ValueTree("SCREEN_SURFACE");

			ssVt.setProperty(IDs::id, id, nullptr);
			ssVt.setProperty(IDs::name, name, nullptr);

			state.appendChild(ssVt, nullptr);

			return ssVt;

		}

		void MudiScreenManager::createNewSurface(const String& newSurfaceName) {

			auto currentName = newSurfaceName;

			auto newID = -1;

			if (currentName.isEmpty()) {

				currentName = generateName();

			}

			if (newID == -1) {

				newID = generateID();

			}

			createSurfaceScreenVt(newID, currentName);

		}

		void MudiScreenManager::createSurface(ValueTree vt) {

			const auto id = vt.getProperty(IDs::id);

			if (surfaceScreensMap.count(id) == 0) {

				auto ss = new MudiSurfaceScreen(vt);

				surfaceScreensMap[id] = std::unique_ptr<MudiSurfaceScreen>(ss);

				save();

			}


		}

		void MudiScreenManager::deleteSurface(ValueTree vt) {

			const int id = vt.getProperty(IDs::id);

			if (surfaceScreensMap.count(id) > 0) {

				surfaceScreensMap.erase(id);

				save();

			}

		}

		void MudiScreenManager::deleteSurface(const int& id) {

			auto childToRemove = state.getChildWithProperty(IDs::id, id);

			if (childToRemove.isValid())

				state.removeChild(childToRemove, nullptr);

		}

		const Displays::Display* MudiScreenManager::getDisplay(int targetDisplay) {

			auto display = Desktop::getInstance().getDisplays().getPrimaryDisplay();

			if (targetDisplay != 0) {
				for (auto& d : Desktop::getInstance().getDisplays().displays) {
					if (!d.isMain)
						display = &d;
				}
			}

			return display;
		}

		Array<Displays::Display> MudiScreenManager::getDisplays() {

			return Desktop::getInstance().getDisplays().displays;

		}

		int MudiScreenManager::getDisplayIndex(const Displays::Display* d) {

			return -1;

		}

		int MudiScreenManager::generateID() {

			auto startID = random.nextInt({ 0,100000 });

			/*Return true if the ID is already present in the array*/
			auto checkForDuplicatedID = [this](const int& startID)->bool {

				return surfaceScreensMap.count(startID) > 0;

			};

			while (checkForDuplicatedID(startID)) {

				startID++;

			}

			return startID;

		}

		String MudiScreenManager::generateName() {

			return "Surface " + std::to_string(surfaceScreensMap.size() + 1);

		}

		void MudiScreenManager::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) {  }

		void MudiScreenManager::valueTreeChildAdded(juce::ValueTree& p, juce::ValueTree& c) {

			if(c.hasType("SCREEN_SURFACE"))

				createSurface(c);

		}

		void MudiScreenManager::valueTreeChildRemoved(juce::ValueTree& p, juce::ValueTree& c, int o) { 
		
			if (c.hasType("SCREEN_SURFACE"))

				deleteSurface(c);
		
		}

		void MudiScreenManager::valueTreeChildOrderChanged(juce::ValueTree&, int, int) { valueTreeChanged(); }


		JUCE_IMPLEMENT_SINGLETON(MudiScreenManager)

	}

}
