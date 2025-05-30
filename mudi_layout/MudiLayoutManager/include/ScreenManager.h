#pragma once

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

#define ScreenManager MudiScreenManager::getInstance()
		class MudiScreenManager;

		class MudiSurfaceScreen : public Selectable {

		public:

			class AttacchedMonitor {

			public:

				AttacchedMonitor(MudiSurfaceScreen* surfaceOwner,int monitorIDx);

				AttacchedMonitor(MudiSurfaceScreen* surfaceOwner, int monitorIDx, Rectangle<int> coveredArea);

				const Displays::Display* getAttacchedDisplay();

				void setCoveredArea(Rectangle<int> newCoveredArea);

				int monitorIDx;
				Rectangle<int> coveredArea;

				MudiSurfaceScreen* surfaceOwner;

			private:

				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AttacchedMonitor)

			};

			~MudiSurfaceScreen();

#if 0
			Array<const Displays::Display*> getAttacchedDisplays();
			const Displays::Display* getAttacchedDisplay(int idx);
#endif // 0


			void attacchDisplay(const Displays::Display* d, juce::Rectangle<int> coveredArea);
			void attacchDisplay(int displayIndex, juce::Rectangle<int> coveredArea);

			void detachDisplay(int displayIndex);

			bool hasAttacchedScreen(int displayIndex);

			AttacchedMonitor* getAttacchedDisplay(int idx);

			juce::String getSelectableDescription();

			inline int getID() { return id; }

			inline String getName() { return name; }

			ValueTree state;

		private:

			friend class MudiScreenManager;

			explicit MudiSurfaceScreen(ValueTree& state);

			String name;

			int id;

			

			OwnedArray<AttacchedMonitor> attacchedMonitors;

		};

		class MudiScreenManager : private DeletedAtShutdown, private ValueTreeAllEventListener {

		public:
			
			~MudiScreenManager();

			/** Return the number of the physical screen */
			int getNumDisplays() {

				return Desktop::getInstance().getDisplays().displays.size();

			}

			/** Return the number of the virtual screen */
			int getNumVirtualScreen() {

				return (int)surfaceScreensMap.size();

			}

			/** Return the surface from a value tree */
			MudiSurfaceScreen* getSurfaceScreenFromVt(const ValueTree& vt);

			/** Return the surface from an ID */
			MudiSurfaceScreen* getSurfaceScreenById(const int& id);

			inline SelectionManager* getSurfaceScreenSelectionManager() {
				
				return surfaceScreenSelectionManager.get();

			}

			/// <summary>
			/// Return an avaiable display from the display user list.
			/// </summary>
			/// <param name="targetDisplay"> The position in the list , 0 is for the main display </param>
			/// <returns> the desidered display ( if present ) or the main display </returns>
			const Displays::Display* getDisplay(int targetDisplay = 0);

			Array<Displays::Display> getDisplays();

			/** Return the index of the display in the JUCE Displays array */
			int getDisplayIndex(const Displays::Display* d);

			/** Create a new suface */
			void createNewSurface(const String& newSurfaceName = "");

			/*Delete a MudiSurfaceScreen from the state. This will perform a valueTreeChildRemoved() that will call deleteSurface().*/
			void deleteSurface(const int& id);

			/** Update the XML stored from the state vt */
			void save();

			ValueTree state;

			JUCE_DECLARE_SINGLETON(MudiScreenManager, true)

		private:

			MudiScreenManager();

			/** Create the state vt and all the MudiSurfaceScreen from the stored XML */
			void load();

			/** Generate a random ID */
			int generateID();

			/*** Genero un nome sequenzile per le nuove surface */
			String generateName();

			/**Creo un nuovo nodo surface sullo state ( questo fa scattare  valueTreeChildAdded() che poi chiama createSurface() */
			ValueTree createSurfaceScreenVt(const int& id, String& name);

			/** Create a MudiSurfaceScreen from it's valueTree */
			void createSurface(ValueTree vt);

			/** Delete a MudiSurfaceScreen from it's valueTree*/
			void deleteSurface(ValueTree vt);

			virtual void valueTreeChanged() override {}

			void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

			void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;

			void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;

			void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;

			std::map<int, std::unique_ptr<MudiSurfaceScreen>> surfaceScreensMap;

			Engine& e;

			std::unique_ptr<XmlElement> surfaceScreensXML;

			std::unique_ptr<SelectionManager> surfaceScreenSelectionManager;

			Random random;

		};

	}


}
