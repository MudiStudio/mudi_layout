namespace mudi {
	
	namespace mudi_layout
	{
		using namespace juce;

		StackLayoutHaeder::StackLayoutHaeder(StackLayout* l) : layout(l) ,
			customComponent(nullptr)
		{

		}

		StackLayoutHaeder::~StackLayoutHaeder() {}


		void StackLayoutHaeder::paint(Graphics& g) {
			LayoutHeaderPanelComponent::paint(g);
		}

		void StackLayoutHaeder::resized() {

			auto area = getLocalBounds().reduced(3).withTrimmedLeft(2).withTrimmedRight(2);
			float margin = 1.0f;
			int buttonSize = area.getHeight();

			for (auto* b : headerButtons) {
				auto localArea = area.removeFromLeft(buttonSize).reduced(1);
				b->setBounds(localArea);
			}

			if (customComponent)
				customComponent->setBounds(area.reduced(1));

		}

		void StackLayoutHaeder::addBtn(const String& componentId, const void* xmlData, size_t numBytes) {

			DrawableBtnComponent * b = new DrawableBtnComponent(true,true);
			
			b->setRadioGroupId(1, dontSendNotification);

			if(xmlData)
				b->setImage(xmlData, numBytes);

			b->setComponentID(STACK_LAYOUT_HEADER_BTN_ID + componentId);

			b->onClick = [this,b] {

				auto* comp = getAssociatedComponent(b);
				setVisibleComponent(comp);
				layout->topComponent = comp;
				layout->topComponentId = comp->getComponentID();
				setCustomComponent();
				layout->resized();
			};
			
			addAndMakeVisible(headerButtons.add(b));
			makeBtnActive();

		}

		void StackLayoutHaeder::setCustomComponent() {
			if (customComponent) removeChildComponent(customComponent);
			if (LayoutManager->isBaseLayoutComponent(layout->topComponent)) {
				if (auto* baseComp = LayoutManager->isBaseLayoutComponent(layout->topComponent)) {
					customComponent = baseComp->getCustomComponentWhenBroughtToTop(layout, layout->topComponent);
					if(customComponent)
						addAndMakeVisible(customComponent);
				}
				
				resized();
			}
		}

		void StackLayoutHaeder::makeBtnActive() {
			if (layout->topComponent) {
				auto* btn = getAssociatedBtn(layout->topComponent);
				btn->triggerClick();
			}
		}

		DrawableBtnComponent* StackLayoutHaeder::getAssociatedBtn(Component* c) {
			for (auto* b : headerButtons) {
				String btnId = b->getComponentID();
				String compId = c->getComponentID();
				if (btnId.replace(STACK_LAYOUT_HEADER_BTN_ID, "").equalsIgnoreCase(compId)) {
					return b;
				}
			}
			jassertfalse;	//No associated header btn !
			return nullptr;
		}

		Component* StackLayoutHaeder::getAssociatedComponent(DrawableBtnComponent* btn) {
			auto compId = btn->getComponentID().replace(STACK_LAYOUT_HEADER_BTN_ID, "");
			return LayoutManager->getComponentByID(compId);
		}

		void StackLayoutHaeder::setVisibleComponent(Component* compToMakeVisible) {
			for (auto* c : layout->getItemsComponent())
				c->setVisible(false);
			compToMakeVisible->setVisible(true);
			compToMakeVisible->toFront(true);
		}

	}


	namespace mudi_layout
	{
		using namespace juce;

		String StackLayout::getLayoutType() {
			return VTIDs::stackLayoutPropertyIdentifier.toString();
		}

		StackLayout::StackLayout(ValueTree& state) : Layout(state), topComponent(nullptr),
			header(new StackLayoutHaeder(this)) 
		{
			addAndMakeVisible(*header);
			header->setVisible(hasHeaderVisible);
			topComponentId.referTo(state, VTIDs::topPropertyIdentifier, nullptr);
		}


		StackLayout::~StackLayout() {}

		void StackLayout::setTopComponent(Component* newTopComponent) {
			topComponent = newTopComponent;
			topComponentId = topComponent->getComponentID();
			auto* btn = header->getAssociatedBtn(topComponent);
			btn->triggerClick();
		}

		Component* StackLayout::addComponent(Component* componentToAdd) {
			//String topComponentId = state.getProperty(VTIDs::topPropertyIdentifier, "append");

			//Se specifico il componente che deve essere in top oppure di dafault in top c'è sempre l'ultimo che aggiungo
			if (topComponentId.get().equalsIgnoreCase(componentToAdd->getComponentID()) || topComponentId.get().equalsIgnoreCase("append"))
				topComponent = componentToAdd;

			Layout::addComponentToLayout(componentToAdd);

			auto iconData = LayoutManager->getIconDataForComponent(componentToAdd);

			header->addBtn(componentToAdd->getComponentID(), iconData.getData(),iconData.getSize());

			return componentToAdd;
		}


		void StackLayout::resized() {

			Layout::resized();

			auto area = getLocalBounds().reduced(margin);
			
			auto currentHeaderH = headerHeight.get() == 0 ? headerHeight.getDefault() : headerHeight.get();

			if(header && header->getParentComponent() == this && header->isVisible())
				header->setBounds(area.removeFromTop(currentHeaderH));

			for (auto* c : comps) {
				if (c->isVisible()) {
					
					area = area.withTrimmedLeft(leftMargin)
						.withTrimmedRight(rightMargin)
						.withTrimmedBottom(bottomMargin)
						.withTrimmedTop(topMargin);

					c->setBounds(area);
				}
			}

		}


	}




}
