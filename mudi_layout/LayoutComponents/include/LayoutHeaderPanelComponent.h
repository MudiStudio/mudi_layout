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

			std::unique_ptr<mudi::mudi_utilities::DrawableComponent> panelIcon;

			float cornerSize = 4.0f;

			Colour mainBackgroundColour, panelHeaderBackground_colour,iconPanelColour;

			bool curveTopLeft = true;
			bool curveTopRight = true;
			bool curveBottomLeft = false;
			bool curveBottomRight = false;
		};
	}
}