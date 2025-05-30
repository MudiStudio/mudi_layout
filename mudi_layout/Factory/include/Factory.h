#ifndef FACTORY_H_INCLUDED
#define FACTORY_H_INCLUDED

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		/**
		 * @class BaseFactory
		 * @brief factory abstract base class that defines its API and is templated on the
		 *        base class of the type that its factories are meant to instantiate.
		 *        Actual factory classes will use the below Factory<Base, Derived>
		 *        version that's defined immediately below.
		 */
		template <class T>
		class BaseFactory
		{
		public:
			/**
			 * @param name The name of the 'type' of object that this factory can create.
			 */
			BaseFactory(StringRef typeName)
				: fTypeName(typeName)
			{
				T::RegisterFactory(typeName, this);
			}

			virtual ~BaseFactory() = default;

			virtual T* Create() = 0;

		protected:
			/// the 'type name' for classes created by this factory, which may or
			/// may not be the same as the C++ name of the classes created by this
			/// factory.
			String fTypeName;

			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseFactory)
		};


	} //dawcomponents
} //microbees

#endif  // FACTORY_H_INCLUDED
