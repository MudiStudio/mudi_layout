namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;


		class StretchMultiViewLayout;

		class StretchMultiViewLayoutHeader : public LayoutHeaderPanelComponent {
		public:

			const String STRETCH_MULTIVIEW_LAYOUT_HEADER_BTN_ID = "STRETCH_MULTIVIEW_LAYOUT_HEADER_BTN_ID_";

			StretchMultiViewLayoutHeader(StretchMultiViewLayout * l);

			OwnedArray<DrawableBtnComponent> headerButtons;

			DrawableBtnComponent* addBtn(const String& componentId, const void* xmlData, size_t numBytes);

			StretchMultiViewLayout* layout;

			void resized() override;

			void paint(Graphics& g) override;

		private:

			int getNumActiveHeaderButtons() const;

#if DEBUG
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StretchMultiViewLayoutHeader);
#endif // DEBUG

		};


		//E' come uno stretch ma presenta un header che permette la visualizzazione di 1 o 2 componente 
		//a seconda dei selettori selezionati
		class StretchMultiViewLayout : public StretchLayout {
		public:


			StretchMultiViewLayout(ValueTree& state);

			String getLayoutType() override;

			Component* addComponent(Component* componentToAdd) override;

			StretchMultiViewLayoutHeader* getStretchMultiViewLayoutHeader();

			CachedValue<bool> showFirst, showSecond;

		private:

#if DEBUG
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StretchMultiViewLayout);
#endif // DEBUG
		};
		
	}
}
