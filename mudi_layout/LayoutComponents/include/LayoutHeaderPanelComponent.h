/*==============================================================================

BaseComponentHeader.h
Created : 24 Mar 2020 8 : 48 : 50am
Author : dicos

==============================================================================
*/
namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		

	}
}


#pragma once
namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		class DrawableBtnComponent : public DrawableButton
		{
		public:

			DrawableBtnComponent(bool clickingToggleState = false, bool drawBackground = true, DrawableButton::ButtonStyle buttonStyle = DrawableButton::ButtonStyle::ImageOnButtonBackground);

			~DrawableBtnComponent();

			void setBackgroundColour(Colour normal);

			void setBackgroundONColour(Colour active);

			void setNormalColours(Colour newIconNormalColour);

			void setOverColours(Colour newOverIconColour);

			void setDownColours(Colour newDownIconColour);

			void setDisableColours(Colour newDisabledIconColour);

			void setImage(const void* xmlData, size_t numBytes);

			void setImageNormal(const void* xmlData, size_t numBytes);

			void setImageOver(const void* xmlData, size_t numBytes);

			void setImageDown(const void* xmlData, size_t numBytes);

			void setImageDisabled(const void* xmlData, size_t numBytes);

			void paintOverChildren(Graphics& g) override;

			void setDrawButtonBackground(bool draw);

		protected:

			//TODO : import theme manager from DAW
			//virtual void themeChanged(const String& themeID) override;

		private:

			std::unique_ptr<Drawable> drawableNormal;

			std::unique_ptr<Drawable> drawableOver;

			std::unique_ptr<Drawable> drawableDown;

			std::unique_ptr<Drawable> drawableDisabled;

			//Colours setted by user
			Colour normalColour;

			Colour overColour;

			Colour downColour;

			Colour disabledColour;

			Colour normalBackgroundColour;

			Colour normalONBackgroundColour;

			//current displaying colour
			Colour currentColour = Colour(0xffffffff);

			//default Colour of SVG
			Colour defaultColour;

			bool drawBackground = true;

			void initColour();

		};


		// ===================== //

		class DrawableComponent : public Component {
		public:

			DrawableComponent() :
				drawable(nullptr)
			{
			}

			void paint(Graphics& g) override {
				if (drawable)
					drawable->drawWithin(g, getLocalBounds().toFloat(), RectanglePlacement::centred, 1.0f);
			}

			inline void setImage(const void* data, const size_t numBytes) {
				drawable = Drawable::createFromImageData(data, numBytes);
			}

			inline void setImageColour(Colour newColour) {
				if (drawable && currentColour != newColour) {
					drawable->replaceColour(currentColour, newColour);
					currentColour = newColour;
					repaint();
				}
			}

		private:
			std::unique_ptr<Drawable> drawable;

			Colour currentColour = Colours::white;

		};


		// ====================== //


		class LayoutHeaderPanelComponent : public Component
		{
		public:

			LayoutHeaderPanelComponent();
			LayoutHeaderPanelComponent(const void* data, const size_t numBytes);
			LayoutHeaderPanelComponent(const String& title, const void* data, const size_t numBytes);
			

			~LayoutHeaderPanelComponent() {};

			void paint(Graphics& g) override;
			void resized() override;

			inline void setConnectionEdges(bool curveTopLeft, bool curveTopRight , bool curveBottomLeft, bool curveBottomRight) {
				 this->curveTopLeft = curveTopLeft;
				 this->curveTopRight = curveTopRight;
				 this->curveBottomLeft = curveBottomLeft;
				 this->curveBottomRight = curveBottomRight;

				 repaint();
			}

			virtual void setPanelIcon(const void* data, const size_t numBytes);

			virtual void setTitle(const String& title);

			Rectangle<int> getAvailableArea();

			void setCornerSize(float newCornerSize);

		protected:
			String title;

			std::unique_ptr<mudi::mudi_layout::DrawableComponent> panelIcon;

			float cornerSize = 4.0f;

			Colour mainBackgroundColour, panelHeaderBackground_colour,iconPanelColour;

			bool curveTopLeft = true;
			bool curveTopRight = true;
			bool curveBottomLeft = false;
			bool curveBottomRight = false;
		};
	}
}