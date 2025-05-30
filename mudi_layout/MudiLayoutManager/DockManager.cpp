namespace mudi
{
	namespace mudi_layout
	{

		// ===================== DockBaseResizableWindow ====================== //

		DockBaseResizableWindow::DockBaseResizableWindow(const String& id, bool closeApp) :
			DocumentWindow(id, Colours::white, DocumentWindow::allButtons,true),
			closeApplicationOnCloseButton(closeApp),
			id(id)
		{

			this->setVisible(true);
			this->setConstrainer(&constrainer);

			constrainer.setMinimumSize(500, 500);
		}

		DockBaseResizableWindow::~DockBaseResizableWindow() {
		}

		void DockBaseResizableWindow::closeButtonPressed() {

			//Must create a local copy of the variable
			auto shouldClose(closeApplicationOnCloseButton);

			DockManager->closeWindow(this);

			if (shouldClose)
				JUCEApplication::getInstance()->systemRequestedQuit();

		}

		void DockBaseResizableWindow::setWindowSize(Rectangle<int> area) {
			this->setTopLeftPosition(area.getX(), area.getY());
			this->setSize(area.getWidth(), area.getHeight());
		}

		void DockBaseResizableWindow::setContent(Component* content, bool owned) {
			if (owned)
				setContentOwned(content, true);
			else
				setContentNonOwned(content, true);
		}


		// ===================== MudiResizableWindow ====================== //


		Layout* MudiResizableWindow::getContentLayout() {
			if (getContentComponent())
				return LayoutManager->isLayoutComponent(getContentComponent());
			
			jassertfalse;	//The content component is not a layout !
			return nullptr;
		}


		// ===================== MudiDockManager ====================== //


		MudiDockManager::MudiDockManager() {
			state = ValueTree(VTIDs::windowsPropertyIdentifier);
		}

		MudiDockManager::~MudiDockManager() {
			closeWindows();
			clearSingletonInstance();
		}

		void MudiDockManager::closeWindows() {

			for (auto* w : windows) {
				w->clearContentComponent();
				w->removeFromDesktop();
			}
			windows.clear();
		}

		void MudiDockManager::closeWindow(DockBaseResizableWindow* windowToClose) {
			windowToClose->removeFromDesktop();
			windows.removeObject(windowToClose);
		}

		MudiResizableWindow* MudiDockManager::createWindowFromVtAndAddContent(ValueTree& vt, Layout* topLayoutContent) {
			//TODO aggiungere la finestra creata agli "all comps" del layout manager
			auto id = vt.getProperty(VTIDs::idPropertyIdentifier).toString();
			int targetScreen = vt.getProperty(VTIDs::windowTargetScreenPropertyIdentifier);

			double proportionOfScreenWidth = abs((double)vt.getProperty(VTIDs::windowDefaultPreferredWidthPropertyIdentifier, -0.5));
			double proportionOfScreenHeight = abs((double)vt.getProperty(VTIDs::windowDefaultPreferredHeigthPropertyIdentifier, -0.5));

			proportionOfScreenWidth = jlimit(0.1,0.9, proportionOfScreenWidth);
			proportionOfScreenHeight = jlimit(0.1, 0.9, proportionOfScreenHeight);

			//TODO : perchè avevo scritto così ?
			//String newCompId = topLayoutContent->getComponentID() + "_screen" + std::to_string(targetScreen);
			String newCompId = topLayoutContent->getComponentID();
			topLayoutContent->setComponentID(newCompId);

			auto* newWin = createWindow(topLayoutContent, id, targetScreen, Point<double>(proportionOfScreenWidth, proportionOfScreenHeight), { -1,-1 },true);
			newWin->state = vt;
			return newWin;
		}

		MudiResizableWindow* MudiDockManager::createWindow(Component* layoutToSet, const String& windowId, int targetScreen, Point<double> proportionOfWindowSize, Point<int> origin, bool closeApplicationOnCloseButton) {


			JamResizableWindowOptions options{
				windowId,
				windowId,
				targetScreen,
				proportionOfWindowSize,
				origin,
				closeApplicationOnCloseButton
			};

			return createWindow(layoutToSet, options);

		}

		MudiResizableWindow* MudiDockManager::createWindow(Component* componentToAdd, const JamResizableWindowOptions& options) {

			MudiResizableWindow* window = new MudiResizableWindow(options.windowName, options.closeApplicationOnCloseButton);

			auto display = &Desktop::getInstance().getDisplays().displays[options.targetScreen];

			auto displayArea = display->userArea;

			RectanglePlacement placement(RectanglePlacement::centred | RectanglePlacement::doNotResize);

			auto area = Rectangle<int>(0, 0, displayArea.getWidth() * (options.windowSize.getX()), displayArea.getHeight() * (options.windowSize.getY()));

			auto result = placement.appliedTo(area, displayArea.reduced(20));

			window->setWindowSize(result);

			componentToAdd->setSize(result.getWidth(), result.getHeight());

			window->setContent(componentToAdd,options.ownedContent);

			windows.add(window);

			if (!options.showTitleBar)
				window->setTitleBarHeight(0);
			else
				window->setUsingNativeTitleBar(options.usingNativeTitleBar);

			window->setResizable(options.isHResizable, options.isVResizable);

			window->id = options.windowID;

			return window;

		}


		MudiResizableWindow* MudiDockManager::getWindowByID(const String& winID) {

			if (auto win = getBaseWindowByID(winID))
				return dynamic_cast<MudiResizableWindow*>(win);

			return nullptr;

		}


		DockBaseResizableWindow* MudiDockManager::getBaseWindowByID(const String& winID) {

			for (auto w : windows)
				if (w->id == winID)
					return w;

			return nullptr;

		}
		


		JUCE_IMPLEMENT_SINGLETON(MudiDockManager);

	}
}