namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		class StretchLayout;

		class JamStretchableLayoutResizerBar : public StretchableLayoutResizerBar {
		public:

			JamStretchableLayoutResizerBar(StretchLayout* ownerLayout, int itemIndexInLayout, bool isBarVertical);

			void hasBeenMoved() override;

			StretchLayout* ownerLayout;
		};


		/*
		Crea un layout Strech verticale o orizzontale.
		LIMITAZIONE : Ogni layout Stretch può contenere elementi a coppia di 2. 
		Ciò significa che se voglio mettere 3 elementi metto il primo , poi creo un altro layout dove metto gli altri 2
		e lo aggiungo al primo così da avere 1 comp + 1 layou ( con 2 comp )
		*/
		class StretchLayout : public Layout {
		public:

			enum Direction {
				LeftToRight = 0,
				TopToBottom
			};

			StretchLayout(ValueTree& state);

			virtual ~StretchLayout();

			Component* addComponent(Component* componentToAdd) override;

			void resized() override;

			bool isVertical() const;

			void compressExpandComponent(Component* targetComponent, bool compress = true);

			bool hasFixedSize(Component * c);

			unique_ptr<LayoutHeaderPanelComponent> header;

			String getLayoutType() override;
			
			void setCollapsedState(bool isCollapsed) override;

			unique_ptr<JamStretchableLayoutResizerBar> dividerBar;

			StretchableLayoutManager stretchableLayout;

			CachedValue<double> stretchResizeBarSize;

			CachedValue<bool> hasHeaderOwnerShip, direction;
			
		protected:


		private:

			const String resizerBarId = "stretchableLayoutResizerBar_";

			int itemLayoutIndex = 0;

			Array<int> itemLayoutIndexArray;

			Array<int> resizeBarLayoutIndexArray;

			double percentualToAbsoluteSize(double percentual, bool isVertical);

		};

	}
}
