namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;
		using namespace std;

#define DockManager MudiDockManager::getInstance()
#define DesktopManager Desktop::getInstance()

		class MudiDockManager;

		class DockBaseResizableWindow : public DocumentWindow {

		public:

			DockBaseResizableWindow(const String& id, bool closeApplicationOnCloseButton = false);
			~DockBaseResizableWindow();

			void setContent(Component* content,bool owned=false);
			void setWindowSize(Rectangle<int> area);
			void closeButtonPressed() override;

			bool closeApplicationOnCloseButton;
			String id;

		protected:

			ComponentBoundsConstrainer constrainer;

		};

		class MudiResizableWindow : public DockBaseResizableWindow, public ApplicationCommandTarget {
		public:

			MudiResizableWindow(const String& id, bool closeApplicationOnCloseButton = false) : DockBaseResizableWindow(id, closeApplicationOnCloseButton){
			
				if (auto cm = Api->getCurrentUIBehaviour().getApplicationCommandManager()) {
					cm->registerAllCommandsForTarget(this);
					addKeyListener(cm->getKeyMappings());
				}
				
			
			}

			~MudiResizableWindow() {
				if (auto cm = Api->getCurrentUIBehaviour().getApplicationCommandManager()) {
					removeKeyListener(cm->getKeyMappings());
				}
			}

			Layout* getContentLayout();

			// ================== //

			virtual ApplicationCommandTarget* getNextCommandTarget() override {
				return nullptr;
			}

			virtual void getAllCommands(Array<CommandID>& commands) {
				Api->getCurrentUIBehaviour().getAllCommands(commands);
			}

			virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override {
				Api->getCurrentUIBehaviour().getCommandInfo(commandID, result);
			}

			virtual bool perform(const InvocationInfo& info) override {
				return Api->getCurrentUIBehaviour().perform(info);
			}

			// =====================//

			ValueTree state;

		};


		class MudiDockManager /* : private DeletedAtShutdown */
		{
		public:

			
			virtual ~MudiDockManager();

			struct JamResizableWindowOptions {

				String windowID;
				String windowName;
				int targetScreen = 0;
				Point<double> windowSize = { 0.5,0.5 };
				Point<int> origin = { -1,-1 };
				bool closeApplicationOnCloseButton = false;
				bool usingNativeTitleBar = false;
				bool showTitleBar = true;
				bool isHResizable = true;
				bool isVResizable = false;
				bool ownedContent = false;

			};

			/// <summary>
			/// Create a dock windows
			/// </summary>
			/// <param name="componentToAdd"> The content component ( it will be owned by the window ) </param>
			/// <param name="windowId"> The ID of the window</param>
			/// <param name="targetScreen"> The index of the screen ( 0 is the main screen )</param>
			/// <param name="windowSize"> The proportion of the screen (0.5 - 0.5 means that the window will have half of the width and half of the heigth of the targetScreen )</param>
			/// <param name="closeApplicationOnCloseButton"> Yes if the close button must close the entire application</param>
			/// <param name="origin"> if (-1,-1) it will be centered on the target screen </param>
			/// <returns> The created window. </returns>
			MudiResizableWindow* createWindow(Component* componentToAdd, const String& windowId, int targetScreen = 0, Point<double> windowSize = { 0.5,0.5 }, Point<int> origin = { -1,-1 }, bool closeApplicationOnCloseButton = false);
			MudiResizableWindow* createWindow(Component* componentToAdd, const JamResizableWindowOptions& options);
			MudiResizableWindow* createWindowFromVtAndAddContent(ValueTree& vt, Layout* content);

			MudiResizableWindow* getWindowByID(const String& winID);
			DockBaseResizableWindow* getBaseWindowByID(const String& winID);

			OwnedArray<DockBaseResizableWindow> windows;

			void closeWindows();
			void closeWindow(DockBaseResizableWindow* windowToClose);

			ValueTree state;
			
			JUCE_DECLARE_SINGLETON(MudiDockManager, true);

		private:

			MudiDockManager();

			
			
		};

	}
}
