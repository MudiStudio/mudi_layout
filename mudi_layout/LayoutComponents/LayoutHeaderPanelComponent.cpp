
namespace mudi
{
	namespace mudi_layout
	{
		
		LayoutHeaderPanelComponent::LayoutHeaderPanelComponent() : title(""),panelIcon(nullptr) {}


		LayoutHeaderPanelComponent::LayoutHeaderPanelComponent(const void* data, const size_t numBytes) : 
			panelIcon(new DrawableComponent()) 
		{
			panelIcon->setImage(data, numBytes);
			addAndMakeVisible(*panelIcon);
		}

		LayoutHeaderPanelComponent::LayoutHeaderPanelComponent(const String& title, const void* data, const size_t numBytes) :
			panelIcon(new DrawableComponent()),
			title(title)
		{
			panelIcon->setImage(data, numBytes);
			addAndMakeVisible(*panelIcon);
		}
		
		void LayoutHeaderPanelComponent::paint(Graphics& g) {

			if (auto* lf = ServiceLocator->getResourcesService()->getCurrentLookAndFeel()) {

				lf->drawPanelHeader(g, getLocalBounds().toFloat());

			}

		}

		void LayoutHeaderPanelComponent::resized() {

			auto area = getLocalBounds().reduced(3).withTrimmedLeft(2).withTrimmedRight(2);
			float margin = 3.0f;
			int buttonSize = area.getHeight();

			if (panelIcon) {
				panelIcon->setBounds(area.removeFromLeft(buttonSize).reduced((int)margin));
			}

		}

		void LayoutHeaderPanelComponent::setPanelIcon(const void* data, const size_t numBytes) {

			if (data == nullptr || numBytes == 0) {
				//L'icona deve avere data e size validi!
				jassertfalse;
				return;
			}

			if (!panelIcon) {
				panelIcon.reset(new DrawableComponent());
			}

			if (auto* lf = ServiceLocator->getResourcesService()->getCurrentLookAndFeel()) {
				
				auto c = lf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::BG1);

				panelIcon->setImageColour(c);

			}

			panelIcon->setImage(data, numBytes);

		}

		void LayoutHeaderPanelComponent::setTitle(const String& title) {
			this->title = title;
		}

		void LayoutHeaderPanelComponent::setCornerSize(float newCornerSize) {
			this->cornerSize = newCornerSize;
			repaint();
		}


		Rectangle<int> LayoutHeaderPanelComponent::getAvailableArea() {
			auto area = getLocalBounds();

			if (panelIcon) {
				area = area.removeFromLeft(panelIcon->getLocalBounds().getWidth());
			}

			return area;



		}

	}
}
