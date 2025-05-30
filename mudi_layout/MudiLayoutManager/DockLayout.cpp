namespace mudi
{
	namespace stem_layout
	{

		MudiResizableWindow::MudiResizableWindow(const String& name, bool addToDesktop) :
			ResizableWindow(name, addToDesktop)
		{

		}


		MudiDockManager::MudiDockManager() {

		}

		MudiDockManager::~MudiDockManager() {

		}

		void MudiDockManager::createWindow(Component* componentToAdd, Point<int> positionOnTheScreen) {

		}


		JUCE_IMPLEMENT_SINGLETON(MudiDockManager);

	}
}