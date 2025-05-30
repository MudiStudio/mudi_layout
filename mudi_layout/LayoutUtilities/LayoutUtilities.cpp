
namespace mudi
{
	namespace mudi_layout
	{

        ShowHideLayoutButton::ShowHideLayoutButton(const String& layoutID, const String& idToSwitch) : layoutID(layoutID), idToSwitch(idToSwitch)
        {
            LayoutManager->addLayoutListener(this);
            startTimerHz(10);
        }

        ShowHideLayoutButton::~ShowHideLayoutButton() {
            LayoutManager->removeLayoutListener(this);
           
        }

        void ShowHideLayoutButton::resized() {

            if (button)
                button->setBounds(getLocalBounds());

        }

        void ShowHideLayoutButton::setButton(Button* b) {

            if (button) {
                button->removeMouseListener(this);
                removeChildComponent(button.get());
                button.reset();
            }

            button.reset(b);

            button->addMouseListener(this, true);
            button->setToggleable(true);
            //button->setClickingTogglesState(true);

            button->onClick = [this]() {

                auto state = button->getToggleState();
                LayoutManager->collapseLayout(layoutID, state, idToSwitch);

            };

            addAndMakeVisible(b);
            resized();

        }

        void ShowHideLayoutButton::timerCallback() {

            auto isCollapsed = LayoutManager->isLayoutCollapsed(layoutID);

            if(button->getToggleState() != !isCollapsed)

                button->setToggleState(!isCollapsed, dontSendNotification);

        }

        void ShowHideLayoutButton::layoutCreated(const String& ld) {

            

        }

        void ShowHideLayoutButton::layoutCollapsed(const String& layoutID, bool isCollapsed)
        {

            timerCallback();

        }


        //---------------------------------------------------------------------------------



        MockLayoutComponent::MockLayoutComponent(const String& text , Colour c) :
            text(text),
            backgroundColour(c)
        {
            this->setName(text);
            this->setComponentID(text);
        }


        void MockLayoutComponent::paint(Graphics& g) {

            auto area = getLocalBounds().toFloat();

            if (backgroundColour != Colours::transparentWhite) {

                g.setColour(backgroundColour);

                g.fillRect(getLocalBounds().toFloat());

                g.setColour(backgroundColour.contrasting());

                g.drawRect(getLocalBounds().toFloat());

            }

            if (text != "") {

                g.setColour(backgroundColour.contrasting());

                g.drawText(text, getLocalBounds(), Justification::centred);

            }
        }

        void MockLayoutComponent::setText(const String& text) {
            this->text = text;
            repaint();
        }

        void MockLayoutComponent::setBackground(Colour c) {
            this->backgroundColour = c;
            repaint();
        }

        void MockLayoutComponent::initFactory() {
            static 	MockLayoutComponentFactory<BaseLayoutComponent,MockLayoutComponent> mockLayoutComponent("MockLayoutComponent");
        }

	}

}