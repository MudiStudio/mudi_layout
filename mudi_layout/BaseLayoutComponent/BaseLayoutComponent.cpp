
namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		BaseLayoutComponent::BaseLayoutComponent() : layout(nullptr)
		{
		}

		BaseLayoutComponent::~BaseLayoutComponent()
		{
		}

		void BaseLayoutComponent::setState(ValueTree& newState) {

			//DBG(newState.toXmlString());
			setOpaque(true);
			this->state = newState;

			id.referTo(state, VTIDs::idPropertyIdentifier, nullptr);
			id.setDefault("");

			jassert(id != "");
			setComponentID(id);

			className.referTo(state, VTIDs::classNamePropertyIdentifier, nullptr);
			className.setDefault("");

			jassert(className != "");

			auto parametersVT = getPropertiesState();

			iconName.referTo(parametersVT, VTIDs::iconNamePropertyIdentifier, nullptr);
			iconName.setDefault("");

			minSize.referTo(parametersVT, VTIDs::defminimumSizePropertyIdentifier, nullptr);
			minSize.setDefault(0);

			maxSize.referTo(parametersVT, VTIDs::defmaximumSizePropertyIdentifier, nullptr);
			maxSize.setDefault(0);

			prefSize.referTo(parametersVT, VTIDs::defPreferredSizePropertyIdentifier, nullptr);
			prefSize.setDefault(0);

            String iconNameStr = iconName.get();
			imageData = getBinaryRes(iconNameStr);

		}

		void BaseLayoutComponent::paintOverChildren(Graphics& g) {

			if (!showEmptyEdit)
				return;

			if (!Api->getCurrentlyFocusedEdit()) {

				auto* lnf = ServiceLocator->getResourcesService()->getCurrentLookAndFeel();
				Colour textColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::Accent);
				Colour bgColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::BG1);
				auto area = getLocalBounds();

				g.setColour(bgColour);
				g.fillRoundedRectangle(area.toFloat(), PADDING_NORMAL);

				g.setColour(textColour);
				g.drawFittedText(TRANS(textToShow), area, Justification::centred, 1, 1.0f);

			}

		}
		
	} //stem_components
} //mudi
