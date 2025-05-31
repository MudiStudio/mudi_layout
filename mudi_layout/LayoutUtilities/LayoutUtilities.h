#pragma once
namespace mudi
{
    namespace mudi_layout
    {

        using namespace juce;

        
        class ShowHideLayoutButton : public Component,private MudiLayoutManager::MudiLayoutListener,private Timer {

        public:

            ShowHideLayoutButton(const String& layoutID, const String& idToSwitch = "");

            ~ShowHideLayoutButton();

            void resized() override;

            void setButton(Button* b);

        private:

            virtual void timerCallback() override;

            virtual void layoutCreated(const String& layoutID) override;
            virtual void layoutCollapsed(const String& layoutID, bool isCollapsed) override;


            std::unique_ptr<Button> button;

            String layoutID;
            String idToSwitch;

        };


        //---------------------------------------------------------------------------------

        class MockLayoutComponent : public BaseLayoutComponent
        {
        public:

            MockLayoutComponent(const String& text = "", Colour c = Colours::red);

            ~MockLayoutComponent() = default;

            void paint(Graphics& g) override;

            void setText(const String& text);

            void setBackground(Colour c);

            static void initFactory();

        private:

            String text;

            Colour backgroundColour = Colours::red;

        };



        template <class T, class MockLayoutComponent>
        class MockLayoutComponentFactory : public BaseFactory<T>
        {
        public:
            MockLayoutComponentFactory(StringRef typeName)
                : BaseFactory<T>(typeName)
            {
                // compile time assertion to make sure that there's a valid
                // base/derived relationship between the two classes that this
                // factory is templated on.
                static_assert(std::is_base_of<T, MockLayoutComponent>::value, "");
            }

            ~MockLayoutComponentFactory() = default;

            T* Create() override
            {

                T* retval = new MockLayoutComponent();

                if (retval)
                {
                    retval->SetTypeName(BaseFactory<T>::fTypeName);
                }

                return retval;
            }
        private:

        };

    }
	
}
