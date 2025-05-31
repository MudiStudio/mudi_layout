namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;


		JamStretchableLayoutResizerBar::JamStretchableLayoutResizerBar(StretchLayout* ownerLayout, 
			int itemIndexInLayout, bool isBarVertical) :
				StretchableLayoutResizerBar(&ownerLayout->stretchableLayout, itemIndexInLayout, isBarVertical),
				ownerLayout(ownerLayout)
		{

		}

		void JamStretchableLayoutResizerBar::hasBeenMoved() {

			StretchableLayoutResizerBar::hasBeenMoved();

			auto* lm = &ownerLayout->stretchableLayout;

			
			auto newPrefSize1 = lm->getItemCurrentRelativeSize(0);
			auto newPrefSize2 = lm->getItemCurrentRelativeSize(2);
			
			auto comp1 = ownerLayout->getItemsComponent()[0];
			auto comp2 = ownerLayout->getItemsComponent()[1];

			LayoutManager->setPreferredSizeForComponent(comp1, newPrefSize1);
			LayoutManager->setPreferredSizeForComponent(comp2, newPrefSize2);
			
		}
	}

	namespace mudi_layout
	{
		using namespace juce;

		String StretchLayout::getLayoutType() {
			return VTIDs::stretchLayoutPropertyIdentifier.toString();
		}

		StretchLayout::StretchLayout(ValueTree& state) : Layout(state), itemLayoutIndex(0) {

			header.reset(new LayoutHeaderPanelComponent(iconData.getData(),iconData.getSize()));
			addAndMakeVisible(*header);
			
			hasHeaderOwnerShip.referTo(state, VTIDs::layoutHeaderOwnershipPropertyIdentifier, nullptr);
			hasHeaderOwnerShip.setDefault(false);

			direction.referTo(state, VTIDs::layoutVerticallyPropertyIdentifier, nullptr);
			direction.setDefault(true);

			stretchResizeBarSize.referTo(state, VTIDs::layoutResizerBarSize, nullptr);
			stretchResizeBarSize.setDefault(6.0);

			if (stretchResizeBarSize.isUsingDefault())
				stretchResizeBarSize.setValue(6.0, nullptr);
			

#if 0
			hasHeaderOwnerShip = state.getProperty(VTIDs::layoutHeaderOwnershipPropertyIdentifier, false);
			stretchResizeBarSize = state.getProperty(VTIDs::layoutResizerBarSize, 6.0);
			direction = state.getProperty(VTIDs::layoutVerticallyPropertyIdentifier, true) ? Direction::TopToBottom : Direction::LeftToRight;
#endif // 0

		}

		StretchLayout::~StretchLayout() {
		}

		bool StretchLayout::hasFixedSize(Component* c) {
			auto sizes = LayoutManager->getSizesForComponent(c);
			return sizes[0] == sizes[1] && sizes[1] == sizes[2];
		}

		Component* StretchLayout::addComponent(Component* componentToAdd) {
			//Aggiungo il primo componente in posizione "0"

			//Aggiungo il secondo componente :
			//	- Se ha una size fisse allora non aggiungo la stretch bar e metto il componente in posizione "1"
			//	- Se ha una size variabile ed il primo componente ha una size fissa allora come sopra
			//	- Se ha una size variabile ed il primo ha una size variabile allora metto lo stretch in "1" ed il componente in "2"

			auto itemSize = getItemsSize();

			if (itemSize > 2) {
				jassertfalse; //Uno stretch layout può avere al più 2 componenti !
				return nullptr;
			}

			bool isFixedSize = hasFixedSize(componentToAdd);// minSize == maxSize && prefSize == maxSize;

			if (itemSize >= 1 && !isFixedSize && !hasFixedSize(getItemsComponent().getFirst())) {

				stretchableLayout.setItemLayout(itemLayoutIndex, stretchResizeBarSize, stretchResizeBarSize, stretchResizeBarSize);
				auto* bar = new JamStretchableLayoutResizerBar(this, itemLayoutIndex, !isVertical());
				bar->setComponentID(resizerBarId + getComponentID() + std::to_string(itemLayoutIndex));
				itemLayoutIndex++;
				dividerBar.reset(bar);
				addAndMakeVisible(*bar);
			}

			if (hasHeaderOwnerShip) {
				auto stackLayout = LayoutManager->isStackLayoutComponent(componentToAdd);
				auto stretchMultiViewLayout = LayoutManager->isStretchMultiViewLayoutComponent(componentToAdd);
				if (stackLayout) {
					stackLayout->removeChildComponent(stackLayout->header.get());
					addAndMakeVisible(stackLayout->header.get());
				}
				else if (stretchMultiViewLayout) {
					stretchMultiViewLayout->removeChildComponent(stretchMultiViewLayout->header.get());
					addAndMakeVisible(stretchMultiViewLayout->header.get());
				}
			}

			auto sizes= LayoutManager->getSizesForComponent(componentToAdd);
			auto minSize = sizes[0];
			auto maxSize = sizes[1];
			auto prefSize = sizes[2];
			stretchableLayout.setItemLayout(itemLayoutIndex++, minSize, maxSize, prefSize);

			return Layout::addComponentToLayout(componentToAdd);
		}


		void StretchLayout::setCollapsedState(bool shouldBeCollapsed)  {
			Layout::setCollapsedState(shouldBeCollapsed);
			if(dividerBar)
				dividerBar->setVisible(!shouldBeCollapsed);
		}

		void StretchLayout::resized() {

			Layout::resized();

			if (getLocalBounds().isEmpty()) return;

			int itemSize = getItemsSize();


			if (itemSize == 0) {
				//jassertfalse;	//C'è un layout senza nessun figlio... non ha senso.
				return;
			}


			Array<Component*> compToLayout;
			auto items = getItemsComponent();
			compToLayout.add(items.getFirst());

			if (itemSize > 1) {

				//TODO se ho 2 elementi ed uno ha una size fissa, non devo metterci la stretch bar in mezzo !

				if (dividerBar && dividerBar->isVisible()) {
					compToLayout.add(dividerBar.get());
				}

				compToLayout.add(items.getLast());

			}


			auto compToLayoutitemSize = compToLayout.size();
			auto area = getLocalBounds().
				reduced(margin)
				.withTrimmedLeft(leftMargin)
				.withTrimmedRight(rightMargin)
				.withTrimmedBottom(bottomMargin)
				.withTrimmedTop(topMargin);

			LayoutHeaderPanelComponent* stackHeader = nullptr;

			for (auto* c : comps) {
				if (LayoutManager->isStackLayoutComponent(c)) {
					stackHeader = LayoutManager->isStackLayoutComponent(c)->header.get();
					break;
				}
				else if (LayoutManager->isStretchMultiViewLayoutComponent(c)) {
					stackHeader = LayoutManager->isStretchMultiViewLayoutComponent(c)->header.get();
					break;
				}
			}

			auto currentHeaderH = headerHeight.get() == 0 ? headerHeight.getDefault() : headerHeight.get();

			if (hasHeaderVisible) {
				if (hasHeaderOwnerShip && stackHeader) {
					stackHeader->setBounds(area.removeFromTop(currentHeaderH));
				}
				else if (header && header->getParentComponent() == this) {
					header->setBounds(area.removeFromTop(currentHeaderH));
				}
			}


			if (compToLayoutitemSize == 1) {		//Ho un solo elemento
				items.getFirst()->setBounds(area);
			}
			else if (items[0]->isVisible() && items[1]->isVisible()) {

				if (compToLayoutitemSize == 2) {	//Ho due elementi fixed ( senza stretch )
					double minSize1, maxSize1, prefSize1;
					double minSize2, maxSize2, prefSize2;

					stretchableLayout.getItemLayout(0, minSize1, maxSize1, prefSize1);
					stretchableLayout.getItemLayout(1, minSize2, maxSize2, prefSize2);

					bool isFixedSize1 = minSize1 == maxSize1 && maxSize1 == prefSize1;
					bool isFixedSize2 = minSize2 == maxSize2 && maxSize2 == prefSize2;

					if (isFixedSize1) {
						minSize1 = percentualToAbsoluteSize(minSize1, isVertical());
						if (isVertical()) {
							minSize1 = percentualToAbsoluteSize(minSize1, true);
							items[0]->setBounds(area.removeFromTop((int)minSize1));
							area.removeFromTop((int)stretchResizeBarSize);	//Padding che simula la stretch
						}
						else {
							minSize1 = percentualToAbsoluteSize(minSize1, false);
							items[0]->setBounds(area.removeFromLeft((int)minSize1));
							area.removeFromLeft((int)stretchResizeBarSize);	//Padding che simula la stretch
						}
						items[1]->setBounds(area);
					}
					else if (isFixedSize2) {
						minSize2 = percentualToAbsoluteSize(minSize2, isVertical());
						if (isVertical()) {
							items[0]->setBounds(area.removeFromTop(area.getHeight() - ((int)minSize2)));
							area.removeFromTop((int)stretchResizeBarSize);	//Padding che simula la stretch
						}
						else {
							items[0]->setBounds(area.removeFromLeft(area.getWidth() - ((int)minSize2)));
							area.removeFromLeft((int)stretchResizeBarSize);	//Padding che simula la stretch
						}
						items[1]->setBounds(area);
					}
				}
				else { 
					//Ho tre elementi (stretch bar in mezzo) tutti visibili
					if (dividerBar)
						dividerBar->setVisible(true);
					stretchableLayout.layOutComponents(compToLayout.getRawDataPointer(), compToLayoutitemSize,
						area.getX(), area.getY(), area.getWidth(), area.getHeight(),
						isVertical(), true);
				}

			}
			else if (items[0]->isVisible() && !items[1]->isVisible()) {
				items[0]->setBounds(area);
				if(dividerBar)
					dividerBar->setVisible(false);

			}
			else if (!items[0]->isVisible() && items[1]->isVisible()) {
				items[1]->setBounds(area);
				if (dividerBar)
					dividerBar->setVisible(false);
			}
		}

		double StretchLayout::percentualToAbsoluteSize(double percentual,bool isVertical) {
			if (percentual > 0) return percentual;
			percentual = jlimit(0.0, 1.0, abs(percentual));
			if (isVertical) {
				return proportionOfHeight((float)percentual);
			}
			else {
				return proportionOfWidth((float)percentual);
			}
		}


		bool StretchLayout::isVertical() const {
			return direction.get() == Direction::TopToBottom;
		}

		void StretchLayout::compressExpandComponent(Component* targetComponent, bool compress) {
			//TODO logica di espansione/compressione
			juce::ignoreUnused(targetComponent);
			juce::ignoreUnused(compress);
		}

	}

}


