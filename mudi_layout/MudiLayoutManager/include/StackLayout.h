
namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;
		using namespace std;

		using namespace mudi::mudi_utilities;

		//class DrawableBtnComponent;
		class StackLayoutHaeder;
		

		class StackLayout : public Layout {
		public:



			StackLayout(ValueTree& state);

			virtual ~StackLayout();

			Component* addComponent(Component* componentToAdd) override;

			void setTopComponent(Component* newTopComponent);

			unique_ptr<StackLayoutHaeder> header;

			void resized() override;

			Component* topComponent;

			String getLayoutType() override;

			CachedValue<String> topComponentId;

		};


		class StackLayoutHaeder : public LayoutHeaderPanelComponent {
		public:

			const String STACK_LAYOUT_HEADER_BTN_ID = "STACK_LAYOUT_HEADER_BTN_ID_";

			StackLayoutHaeder(StackLayout* l);

			~StackLayoutHaeder();

			void paint(Graphics& g) override;

			void resized() override;

			void addBtn(const String& componentId, const void* xmlData, size_t numBytes);

			StackLayout* layout;

			void makeBtnActive();

			DrawableBtnComponent* getAssociatedBtn(Component* c);

			Component* getAssociatedComponent(DrawableBtnComponent* btn);

			OwnedArray<DrawableBtnComponent> headerButtons;

			Component* customComponent;

			void setVisibleComponent(Component* compToMakeVisible);

		private:

			void setCustomComponent();

		};

		

		
	}
}
