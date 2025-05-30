#pragma once

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		class Layout : public Component {
		public:

			static const Identifier layoutIdentifier;

			static const int MAX_SLOTS;

			Layout(ValueTree& state);

			virtual ~Layout();

			void paint(Graphics& g) override;

			void resized() override;

			//Le sottoclassi devono implemetare tale metodo che permette l'aggiunta di un componente al layout
			virtual Component* addComponent(Component* componentToAdd) = 0;

			//Ritorna il tipo del layout [Stretch,Stack,ecc...]
			virtual String getLayoutType() = 0;

			//Cerca di trovare ricorsivamente un componente nella gerarchia
			Component* findComponentByIdRecursive(const String& id, Layout* parent = nullptr);
			
			//Cerca di trovare ricorsivamente un layout nella gerarchia
			Layout* findLayoutByComponentIdRecursive(const String& id);
			
			int getItemsSize() const;
			
			Array<Component*> getItemsComponent();
			
			Layout* getParentLayout();

			Array<Layout*> getChildrenLayout();

			Layout* getChildLayout(const String& layoutID);

			virtual void setCollapsedState(bool shouldBeCollapsed);

			bool isCollapsedState();
			
			const String getID();

			CachedValue<double> minSize, prefSize, maxSize;
			
			CachedValue<bool> isCollapsed, hasHeaderVisible, isCollapsible;
			
			CachedValue<String> id, iconName;

			CachedValue<int> miniatureSlots;

			CachedValue<int> headerHeight;

			CachedValue<int> topMargin, bottomMargin, leftMargin, rightMargin,margin;

			MemoryBlock iconData;

			ValueTree state;

		protected:
	
			std::unique_ptr<Button> backNavigationButton;

			Component* addComponentToLayout(Component* componentToAdd);

			void setParent(Component* comp);

			OwnedArray<Component> comps;
			
			Component* parent;

			Colour colourWhenEmpty;

		private:

#if DEBUG
			JUCE_LEAK_DETECTOR(Layout)
#endif

		};
	}
}


#if 0

#include "StackLayout.h"
#include "StretchLayout.h"
#include "DockManager.h"
#include "StretchMultiViewLayout.h"  
#endif // 0


namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;
		using namespace std;

		class MudiLayoutEvent;

		class MudiEventProcessor {
		public:

			MudiEventProcessor();

			~MudiEventProcessor();

			void resetProcessor();

			void addEvent(MudiLayoutEvent* e, const String& eventID);

			void fireEvent(const String& eventType, const String& className, const String& eventId = "");

			void setState(ValueTree state);

			ValueTree state;

		private:

			void fireSelectedEvent(const String& className, const String& eventID);

			void fireUserEvent(const String& className, const String& eventID);

			void fireEventNotifyingListener(MudiLayoutEvent* e);

			map<String, MudiLayoutEvent*> eventsMap;

			OwnedArray< MudiLayoutEvent> events;
		};

		class MudiLayoutEvent {
		public:

			MudiLayoutEvent(const String& id ,const String& className);
            
            virtual ~MudiLayoutEvent();

			virtual bool trigger() = 0;

			virtual String getType() = 0;

			virtual void addEvent(MudiLayoutEvent* e, const String& eventType);

			String className;

			String id;

			ValueTree state;

		};


		class MudiLayoutUserEvent : public MudiLayoutEvent {
		public:

			MudiLayoutUserEvent(const String& id, const String& className);

			virtual ~MudiLayoutUserEvent();

			bool trigger() override;

			String getType() override;

			void addEvent(MudiLayoutEvent* e, const String& eventID) override;

			Array<MudiLayoutEvent*> subEvents;

		};

		class JamLayoutSelectedEvent : public MudiLayoutEvent {
		public:

			JamLayoutSelectedEvent(const String& id, const String& className, const String& target, const String& action);
			
			virtual ~JamLayoutSelectedEvent();

			bool trigger() override;

			String getType() override;

			String target, action;

		};
	}
}

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;
		using namespace std;
		
#define LayoutManager MudiLayoutManager::getInstance()

		class MudiLayoutManager : /*private DeletedAtShutdown,*/ public ChangeListener, private mudi::mudi_project::MudiEditManager::Listener {
		public:

			virtual ~MudiLayoutManager();

			// ------------------------------- //

			Component* buildLayout(const String layoutType = "0");

			/* @Internal */
			Component* createLayout();

			/* @Internal */
			Layout* createLayoutFromVt(ValueTree& v);

			/* @Internal */
			StackLayout* createStackLayoutFromVt(ValueTree& state);

			/* @Internal */
			StretchLayout* createStretchLayoutFromVt(ValueTree& state);

			/* @Internal */
			StretchMultiViewLayout* createStretchMultiViewLayoutFromVt(ValueTree& state);

			/* @Internal */
			BaseLayoutComponent* createBaseLayoutComponentFromVt(ValueTree& vt);

			// ------------------------------- //

			MemoryBlock getIconDataForComponent(const String& componentID);

			MemoryBlock getIconDataForComponent(Component* comp);

			// ------------------------------- //

			/// <summary>
			/// Ritorna una mappa di [ID,Component] che rappresenta la lista di tutti i figli ( BaseDawComponent oppure Layout ) del parent 
			/// </summary>
			/// <param name="parent"> root dalla quale costruire l'alberatura dei figli</param>
			/// <param name="map"> riferimento ad una mappa che verrà popolata</param>
			void getAllChildrenFromLayoutRecursive(Layout* parent, map<String, Component*>& map);

			StretchLayout* getDawLayout();

			StretchLayout* getDJLayout();

			void getAllChildrenFromDawLayout(map<String, Component*>& map);

			void getAllChildrenFromDjLayout(map<String, Component*>& map);

			void getAllChildrenFromScreen(int screenNum,map<String,Component*>& map);

			// ------------------------------- //

			Component* getComponentByID(const String& id);

			ValueTree getStateFromComponent(Component* c);

			// ------------------------------- //

			void buildMiniatureLayout(StretchLayout* parent, Rectangle<int>& area, std::map<String, Rectangle<int>>& map);
			
			void buildMiniatureLayoutForDaw(Rectangle<int>& area, std::map<String, Rectangle<int>>& map);
			
			void buildMiniatureLayoutForDj(Rectangle<int>& area, std::map<String, Rectangle<int>>& map);
			
			void buildMiniatureLayoutForScreen(int screenNum, Rectangle<int>& area, std::map<String, Rectangle<int>>& map);

			void fireUserEvent(const String& eventId);

			// ------------------------------- //

			/**
			 * If the "id" belongs to a layout that is the last one visibile in it's parent and the parent is not collapsible you should pass an "idToSwitch" to remaining an empty space. 
			 * if "idToSwitch" is null and there will be an empty state the layout with id "id" will not be collapsed :/
			 */
			void collapseLayout(const String& id, bool shoudBeCollapsed, const String& idToSwitch = "");

			bool isLayoutCollapsed(const String& id);

			void setTopComponent(const String& id, bool shoudBeVisible);

			bool isTopComponent(const String& id);

			// ------------------------------- //

			BaseLayoutComponent* isBaseLayoutComponent(Component* componentToCheck);

			Layout* isLayoutComponent(Component* componentToCheck);

			StackLayout* isStackLayoutComponent(Component* componentToCheck);

			StretchLayout* isStretchLayoutComponent(Component* componentToCheck);

			StretchMultiViewLayout* isStretchMultiViewLayoutComponent(Component* componentToCheck);

			// ------------------------------- //

			ValueTree getEventsVt();

			ValueTree getScreensVt();

			// ------------------------------- //

			void saveLayout();

			void resetLayout();
			
			void clearLayout();

			map<String, Component*> getAllComps();

			// ------------------------------- //

			// @Internal Return an array composed by component's [min,max,pref] size, or an empty array if sizes are not present.
			Array<double> getSizesForComponent(ValueTree& vt);

			/* @Internal */
			Array<double> getSizesForComponent(Component* c);

			/* @Internal */
			void setPreferredSizeForComponent(Component* c,double newPrefSize);

			/* @Internal */
			void setMinSizeForComponent(Component* c,double newMinSize);

			/* @Internal */
			void setMaxSizeForComponent(Component* c,double newMaxSize);

			// ------------------------------- //
			
			MudiEventProcessor& getEventProcessor();

			// ------------------------------- //

			const String getCurrentLayoutType();

			//Ritorna una mappa di nome_file|nome_layout (e.g [ "layout_0.xml" | "ciao sono un layout" ] )
			const std::map<String, String> getLayoutFileNameMap();

			// ========== LayoutListener ==================== //
			
			class MudiLayoutListener {
			public:

				virtual ~MudiLayoutListener() = default;

				virtual void layoutCreated(const String& layoutID) {}

				virtual void layoutChanged(const String& oldLayoutID, const String& newLayoutID) {}

				virtual void layoutCollapsed(const String& layoutID, bool isCollapsed) {}

			};

			void addLayoutListener(MudiLayoutListener* newListener) {
				layoutListeners.add(newListener);
			}

			void removeLayoutListener(MudiLayoutListener* listener) {
				layoutListeners.remove(listener);
			}

			// ========== /LayoutListener ==================== //

			// ========== EventListener ==================== //

			class MudiLayoutEventListener {
			public:

				virtual ~MudiLayoutEventListener() = default;

				virtual void preTrigger(MudiLayoutEvent*) = 0;

				virtual void postTrigger(MudiLayoutEvent*) = 0;

			};

			void addEvProcListener(MudiLayoutEventListener* newListener) { 
				eventProcListeners.add(newListener); 
			}
			
			void removeEvProcListener(MudiLayoutEventListener* listener) {
				eventProcListeners.remove(listener); 
			}

			void sendPreTrigger(MudiLayoutEvent* e){
				eventProcListeners.call([e, this](MudiLayoutEventListener& l) { l.preTrigger(e); });
			}

			void sendPostTrigger(MudiLayoutEvent* e){
				eventProcListeners.call([e, this](MudiLayoutEventListener& l) { l.postTrigger(e); });
			}

			// ========== /EventListener ==================== //

			Layout* mainLayout;

			ValueTree state;

			JUCE_DECLARE_SINGLETON(MudiLayoutManager, true);

		protected:

			void changeListenerCallback(ChangeBroadcaster* source) override;

			//Call this as a setUp method ( you must create all objs here )
			virtual void editChanged() override;

			//Call this as a tearDown method ( you must destroy all the objs here )
			virtual void editIsAboutToBeChanged(Edit* editThatIsGoingToChange, Edit* newSelectedEdit) override;

		private:

			friend class Layout;

			//Il singleton vuole un costruttore private prefibilmente di modo da assicurare il non-instaziamento diretto della classe
			MudiLayoutManager();

			Component* buildLayout(const File& appPrefs, const char* resource, int file_size, const String& xmlFile, const String& layoutType);

			void buildLayoutRecursive(Layout*parent, ValueTree& vt);

			void buildScreens(ValueTree& screensVt);

			//Controllo se il
			bool loadedFromFile(const File& xmlFile);

			void createLayoutFromResource(const char* resource, int file_size, const File& layoutFile);

			//Se isStretch è false il secondo parametro è ignorato
			void createMainLayout();

			//Controlla se ci sono dei layout da collassare
			void checkForCollapsedLayout();

			//Controlla se al termine della build del layout sono presenti i nodi "DawLayout" e "DjLayout"
			void checkForDawAndDjLayout();

			void registerComponent(Component* compToRegister, String compId);

			void layoutCreatedCallback(const String& layoutID);

			void layoutChangedCallback(const String& oldLayoutID, const String& newLayoutID);

			void layoutCollapsedCallback(const String& layoutID,bool isCollpased);

			/// <summary>
			/// Torna la lista dei layout raw 
			/// Esempio : se ci sono 2 file "layout_1" e "layout_2" viene restituita la lista "[layout_1.xml, layout_2.xml]"
			/// </summary>
			/// <returns></returns>
			StringArray getAvailableRawLayoutList();

			/// <summary>
			/// Popola una mappa<String,String> composta dal nome del file di layout (e.g "layout_0.xml")
			/// e dal nome settato nella proprità "name" sul nodo "ROOT" (e.g "standard layout")
			/// </summary>
			/// <returns></returns>
			void buildLayoutList();

			void createEvent(ValueTree& eventVt);

			void initEvent(ValueTree& eventVt, MudiLayoutUserEvent* userEvent);

			void initChangeListener();

			void resetChangeListener();

			/** The version has a format kind like "1.2.3", so this method will split it and return the sum of the values */
			int getIntegerLayourVersion(const String& resourceLayoutVersion);

			std::map<String, String> layoutFileNameMap;

			ListenerList<MudiLayoutEventListener> eventProcListeners;

			ListenerList<MudiLayoutListener> layoutListeners;

			MudiEventProcessor eventProcessor;

			int currentLayoutVersion;

			File xmlLayout;

			String currentLayoutType;

			//Mi registro tutti i componenti in una mappa così da effettuare operazioni di ricerca più velocemente
			map < String, Component* > allComps;

		};


	}
}



