#ifndef BASE_STAM_COMP_H_INCLUDED
#define BASE_STAM_COMP_H_INCLUDED

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;
		using namespace mudi;
		using namespace mudi_utilities;

		/**
		 * @class BaseLayoutComponent
		 * @brief A class for the class factory system.
		 *
		 * The 3 methods:
		 * - `SetTypeName()``
		 * - `RegisterFactory()`
		 * - `Create()`
		 *
		 * must be present in the base of a class that is to be created using this
		 * class factory system.
		 */
		class BaseLayoutComponent : public FactoryBuildable<BaseLayoutComponent>, public Component
		{
		public:
			/**
			 * The classes created by our factory need to be default-constructable.
			 */
			BaseLayoutComponent();

			virtual ~BaseLayoutComponent();

			virtual void setKeyMode(bool isOn) {}
			
			void setState(ValueTree& newState);

			void showEmptyEditsScreen(bool show) {
				showEmptyEdit = show;
				repaint();
			}

			void setEmptyEditText(const String& textToShow) {
				this->textToShow = textToShow;
				repaint();
			}

			void paintOverChildren(Graphics& g) override;

			ValueTree getState() {
				return state; 
			}

			ValueTree getPropertiesState() { 
				return state.getChildWithName(mudi_utilities::VTIDs::componentPropertiesIdentifier);
			}

			virtual void addProperty(ValueTree property)
			{
				auto parametersVT = getPropertiesState();

				if (parametersVT.isValid()) {
					parametersVT.copyPropertiesAndChildrenFrom(property, NULL);

					if (parametersVT.hasProperty(mudi_utilities::VTIDs::iconNamePropertyIdentifier)) {
						auto iconName = parametersVT.getProperty(mudi_utilities::VTIDs::iconNamePropertyIdentifier, "").toString();
						//imageData = getBinaryRes(iconName);
					}

				}
			}

			virtual int getDawComponentLayoutModeProperty() {
				String propName = "layoutMode";
				return (int)getDawComponentProperty(propName);
			}

			virtual var getDawComponentProperty(const var& name)
			{
				auto parametersVT = getPropertiesState();

				if (parametersVT.isValid()) {
					ValueTree prop = parametersVT.getChildWithProperty(mudi_utilities::VTIDs::propertyNameIdentifier, name);
					if (prop.isValid())
					{
						return prop.getProperty(mudi_utilities::VTIDs::propertyValueIdentifier);
					}
				}

				return var();
			}

			virtual void initDawComponent() {}

			mudi_layout::Layout* layout;

			//Callback quando un componente di uno stack viene portato in front per ottenere la sua bottoniera custom
			virtual Component* getCustomComponentWhenBroughtToTop(Layout* layout, Component* component) { return nullptr;  };

			virtual Button* getBackNavigationButton(Layout* layout, Component* component) { return nullptr; };

			//Callback quando un componente di uno stack viene portato in back
			virtual void broghtToBack(Layout* layout, Component* component) {};

			MemoryBlock& getIconData() { return imageData; }

			void setImageData(const void* data, size_t numBytes) { imageData.replaceAll(data, numBytes); }

			String getId() const { return id.get().toStdString(); }

			CachedValue<double> minSize, maxSize, prefSize;

			CachedValue<String> iconName,id,className;

		protected:

			juce::ValueTree state;

			MemoryBlock imageData;

		private:

			bool showEmptyEdit = true;
			String textToShow = "_editlist_empty_message_";

			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseLayoutComponent)
		};

	} //stem_components
} //mudi

#endif  // BASEDC_H_INCLUDED
