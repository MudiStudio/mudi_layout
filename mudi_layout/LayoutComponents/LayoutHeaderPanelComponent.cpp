
namespace mudi
{
	namespace mudi_layout
	{
		

        DrawableBtnComponent::DrawableBtnComponent(bool clickingToggleState, bool drawBackground, DrawableButton::ButtonStyle buttonStyle) :
            DrawableButton("", buttonStyle),
            drawableNormal(nullptr),
            drawableOver(nullptr),
            drawableDown(nullptr),
            drawableDisabled(nullptr),
            drawBackground(drawBackground)
        {

            initColour();

            setClickingTogglesState(clickingToggleState);

            defaultColour = Colours::black;

            //TODO_UMB
            normalColour = Colours::white; //ServiceLocator->getResourcesService()->getLookAndFeelColour(mudi_lnf::MudiLookAndFeel::ColourScheme::defaultText);

            overColour = normalColour.brighter(0.2f);

            //TODO_UMB
            downColour = Colours::white;// ServiceLocator->getResourcesService()->getLookAndFeelColour(mudi_lnf::MudiLookAndFeel::ColourScheme::buttonBackgroundONColour);

            disabledColour = normalColour.darker(0.2f);

        }

        DrawableBtnComponent::~DrawableBtnComponent() {

        }

        void DrawableBtnComponent::setBackgroundColour(Colour normal) {
            this->setColour(DrawableButton::ColourIds::backgroundColourId, normalBackgroundColour);
        }

        void DrawableBtnComponent::setBackgroundONColour(Colour active) {
            this->setColour(DrawableButton::ColourIds::backgroundOnColourId, active);
        }

        void DrawableBtnComponent::setNormalColours(Colour newIconNormalColour) {
            drawableNormal->replaceColour(normalColour, newIconNormalColour);
            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setOverColours(Colour newOverIconColour) {
            drawableOver->replaceColour(overColour, newOverIconColour);
            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setDownColours(Colour newDownIconColour) {
            drawableDown->replaceColour(downColour, newDownIconColour);
            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setDisableColours(Colour newDisabledIconColour) {
            drawableDisabled->replaceColour(disabledColour, newDisabledIconColour);
            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setImage(const void* xmlData, size_t numBytes) {
            bool changed = false;

            drawableNormal = Drawable::createFromImageData(xmlData, numBytes);
            changed = drawableNormal->replaceColour(defaultColour, normalColour);

            drawableOver = Drawable::createFromImageData(xmlData, numBytes);
            drawableOver->replaceColour(defaultColour, overColour);

            drawableDown = Drawable::createFromImageData(xmlData, numBytes);
            drawableDown->replaceColour(defaultColour, downColour);

            drawableDisabled = Drawable::createFromImageData(xmlData, numBytes);
            drawableDisabled->replaceColour(defaultColour, disabledColour);

            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setImageOver(const void* xmlData, size_t numBytes) {
            drawableOver = Drawable::createFromImageData(xmlData, numBytes);
            drawableOver->replaceColour(defaultColour, overColour);

            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setImageNormal(const void* xmlData, size_t numBytes) {
            drawableNormal = Drawable::createFromImageData(xmlData, numBytes);
            drawableNormal->replaceColour(defaultColour, normalColour);

            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setImageDown(const void* xmlData, size_t numBytes) {
            drawableDown = Drawable::createFromImageData(xmlData, numBytes);
            drawableDown->replaceColour(defaultColour, downColour);

            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::setImageDisabled(const void* xmlData, size_t numBytes) {
            drawableDisabled = Drawable::createFromImageData(xmlData, numBytes);
            drawableDisabled->replaceColour(defaultColour, disabledColour);

            setImages(drawableNormal.get(), drawableOver.get(), nullptr, drawableDisabled.get(), drawableDown.get());
        }

        void DrawableBtnComponent::paintOverChildren(Graphics& g) {
            juce::ignoreUnused(g);
        }

        void DrawableBtnComponent::setDrawButtonBackground(bool draw) {
            drawBackground = draw;
            initColour();
            repaint();
        }

#if 0
        void DrawableBtnComponent::themeChanged(const String& themeID) {
            initColour();
        }
#endif // 0


        void DrawableBtnComponent::initColour() {

            //TODO_UMBù
#if 0
            auto* lnf = ServiceLocator->getResourcesService()->getCurrentLookAndFeel();

            normalColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::buttonContentColour);
            overColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::buttonContentColour);
            downColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::buttonContentONColour);
            disabledColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::buttonContentDISABLEDColour);

            normalBackgroundColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::buttonBackgroundColour);
            normalONBackgroundColour = lnf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::buttonBackgroundONColour);
#endif // 0


            if (!drawBackground) {
                this->setColour(DrawableButton::ColourIds::backgroundColourId, Colours::transparentWhite);
                this->setColour(DrawableButton::ColourIds::backgroundOnColourId, Colours::transparentWhite);
            }
            else {
                this->setColour(DrawableButton::ColourIds::backgroundColourId, normalBackgroundColour);
                this->setColour(DrawableButton::ColourIds::backgroundOnColourId, normalONBackgroundColour);
            }

        }

		// ============================================== //


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

			//@@@ TODO_UMB
#if 0
			if (auto* lf = ServiceLocator->getResourcesService()->getCurrentLookAndFeel()) {

				lf->drawPanelHeader(g, getLocalBounds().toFloat());

			}
#endif // 0


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

			//@@@ TODO_UMB
#if 0
			if (auto* lf = ServiceLocator->getResourcesService()->getCurrentLookAndFeel()) {

				auto c = lf->getCurrentColourScheme().getUIColour(mudi::mudi_lnf::MudiLookAndFeel::ColourScheme::BG1);

				panelIcon->setImageColour(c);

			}
#endif // 0


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
