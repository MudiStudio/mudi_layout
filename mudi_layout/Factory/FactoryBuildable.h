
#ifndef FACTORYBUILDABLE_H_INCLUDED
#define FACTORYBUILDABLE_H_INCLUDED

namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;



		template <class T>
		class FactoryBuildable
		{
		public:
			FactoryBuildable()
				: fTypeName("!!UNKNOWN!!")
			{

			}

			virtual ~FactoryBuildable() {}

			/**
			 * Retuired for the factory -- set the typename used to create an object. By
			 * storing it redundantly in each object we can easily make sure that we
			 * persist each object derived from Base correctly.
			 * @param name Name of this type, which may not match the C++ class name.
			 */
			void SetTypeName(StringRef name)
			{
				fTypeName = name;
			}

			/**
			 * Register a Factory object with our base class so we can instantiate
			 * Base objects by using their class name, like:
			 * ```
			 * Base* n = Base::Create("myClassName");
			 * ```
			 * @param name    name of the class to use when creating one (not necessarily
			 *                the same as the actual C++ class name)
			 * @param factory Pointer to a BaseFactory object that will return an instance
			 *                of the Base class.
			 */
			static void RegisterFactory(StringRef name, BaseFactory<T>* factory)
			{
				FactoryDatabase<T>::RegisterFactory(name, factory);
			}
			/**
			 * Create a Base object using its name.
			 * @param  name Name of the Base class you'd like to create.
			 * @return      A pointer to the object if we have a registered factory
			 *              object that knows how to make it, or `nullptr`.
			 */
			static T* Create(StringRef name)
			{
				return FactoryDatabase<T>::Create(name);
			}

			static void InitFactories()
			{
				FactoryDatabase<T>::resetDatabase();
			}

			String fTypeName;
		private:
			

		};

	} //dawcomponents
} //microbees
#endif  // FACTORYBUILDABLE_H_INCLUDED
