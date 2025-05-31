namespace mudi
{

	// ==================== StretchMultiViewLayoutHeader ====================== //
	namespace mudi_layout
	{

		StretchMultiViewLayoutHeader::StretchMultiViewLayoutHeader(StretchMultiViewLayout* l) : layout(l) {
			
		}

		DrawableBtnComponent* StretchMultiViewLayoutHeader::addBtn(const String& componentId, const void* xmlData, size_t numBytes) {
			DrawableBtnComponent* b = new DrawableBtnComponent();

			b->setToggleState(true, dontSendNotification);

			if (xmlData)
				b->setImage(xmlData, numBytes);

			b->setComponentID(STRETCH_MULTIVIEW_LAYOUT_HEADER_BTN_ID + componentId);

			b->onClick = [this, b] {

				//Se ci sono 2 bottoni attivi tutto ok
				//Se c'è un bottone attivo e l'altro off: 
					//	Se clicclo sul bottone attivo switch --> non posso avere entrambi i bottoni off!
					//	Se clicclo sull'altro bottone tutto ok

				if (getNumActiveHeaderButtons() == 1 && b->getToggleState()) {
					//TODO : In questo caso metto in primo piano l'altro layout
					return;

				}
					

				b->setToggleState(!b->getToggleState(), dontSendNotification);

				auto* comp = LayoutManager->getComponentByID(b->getComponentID().replace(STRETCH_MULTIVIEW_LAYOUT_HEADER_BTN_ID, ""));

				auto toggleState = b->getToggleState();

				comp->setVisible(toggleState);
				if(layout->dividerBar)
					layout->dividerBar->setVisible(toggleState);


				layout->resized();

				auto compID = b->getComponentID().replace(STRETCH_MULTIVIEW_LAYOUT_HEADER_BTN_ID, "");

				if (layout->getItemsComponent().getFirst()->getComponentID() == compID) {
					layout->showFirst = b->getToggleState();
				}
				else if (layout->getItemsComponent().getLast()->getComponentID() == compID) {
					layout->showSecond = b->getToggleState();
				}

			};

			addAndMakeVisible(*b);

			headerButtons.add(b);

			return b;
		}

		void StretchMultiViewLayoutHeader::paint(Graphics& g) {

			LayoutHeaderPanelComponent::paint(g);

		}

		void StretchMultiViewLayoutHeader::resized() {

			auto area = getLocalBounds().reduced(3).withTrimmedLeft(2).withTrimmedRight(2);
			
			int buttonSize = area.getHeight();

			for (auto* b : headerButtons) {
				auto localArea = area.removeFromLeft(buttonSize).reduced(1);
				b->setBounds(localArea);
			}
			
		}

		int StretchMultiViewLayoutHeader::getNumActiveHeaderButtons() const {
			int num = 0;
			for (auto* b : headerButtons) {
				if (b->getToggleState()) 
					num++;
			}
			return num;
		}


	}



	// ==================== StretchMultiViewLayout ====================== //
	namespace mudi_layout
	{
		
		StretchMultiViewLayout::StretchMultiViewLayout(ValueTree& state) : StretchLayout(state) 
		{
			header.reset(new StretchMultiViewLayoutHeader(this));
			
			hasHeaderVisible = true;

			showFirst.referTo(state, VTIDs::stretchMultiViewShowFirstPropertyIdentifier, nullptr);
			if (showFirst.isUsingDefault())
				showFirst.setValue(true, nullptr);

			showSecond.referTo(state, VTIDs::stretchMultiViewShowSecondPropertyIdentifier, nullptr);
			if (showSecond.isUsingDefault())
				showSecond.setValue(true, nullptr);

			if (!showFirst && !showSecond) {
				jassertfalse;	//Almeno uno tra i due componenti deve essere mostrato !
				showFirst.setValue(true, nullptr);
				showSecond.setValue(true, nullptr);
			}

			addAndMakeVisible(*header);

		}

		String StretchMultiViewLayout::getLayoutType(){
			return VTIDs::stretchMultiViewLayoutPropertyIdentifier.toString();
		}

		Component* StretchMultiViewLayout::addComponent(Component* componentToAdd) {
			auto* c = StretchLayout::addComponent(componentToAdd);
			auto iconData = LayoutManager->getIconDataForComponent(componentToAdd);
			auto * b = getStretchMultiViewLayoutHeader()->addBtn(componentToAdd->getComponentID(), iconData.getData(), iconData.getSize());

			if ((getItemsSize() == 1 && !showFirst) || (getItemsSize() == 2 && !showSecond))
				b->triggerClick();

			return c;
		}


		StretchMultiViewLayoutHeader* StretchMultiViewLayout::getStretchMultiViewLayoutHeader() {
			return dynamic_cast<StretchMultiViewLayoutHeader*>(header.get());
		}

	}

}


