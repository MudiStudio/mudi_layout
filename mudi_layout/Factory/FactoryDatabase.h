#ifndef FACTORYDATABASE_H_INCLUDED
#define FACTORYDATABASE_H_INCLUDED



 /**
  * @class FactoryDatabase
  *
  * @brief A class that lets lets us associate Node classes with a name that
  *        can be used to instantiate them at runtime.
  *
  * Some interesting and probably non-obvious code in this one. The issue that we
  * run into here is that we want each of the Node classes to register itself
  * with the single static instance of this class so that we can then create it.
  * We want this registration to happen before the execution of `main()`, but
  * we immediately run into the problem that the order of initialization between
  * different translation units is undefined behavior, so it's possible (and in
  * fact likely or almost certain) that the Node classes will try to register
  * themselves before the map of names to factory functions has been created.
  *
  */
namespace mudi
{
	namespace mudi_layout
	{
		using namespace juce;

		
		template <class T>
		class FactoryDatabase
		{
		public:
            

			FactoryDatabase()
			{
				if (0 == fCount++)
				{
					fDatabase = new HashMap<String, BaseFactory<T>* >();
				}

				
			}

			~FactoryDatabase()
			{
				if (0 == --fCount)
				{
					delete fDatabase;
					fDatabase = nullptr;
				}
			}

			/**
			 * Register a Node Factory object with a name that can be used to create
			 * Node objects at run time.
			 * @param name    name of the class to use when creating one (not necessarily
			 *                the same as the actual C++ class name)
			 * @param factory Pointer to a NodeFactory object that will return an instance
			 *                of the Node class.
			 */
			static void RegisterFactory(StringRef name, BaseFactory<T>* factory)
			{
				jassert(fDatabase);
				DBG("Registering factory for " << name);
				fDatabase->set(name, factory);
			}
			/**
			 * Create a Node object using its name.
			 * @param  name Typename of the class you'd like to create.
			 * @return      A pointer to the object if we have a registered factory
			 *              object that knows how to make it, or `nullptr`.
			 */
			static T* Create(StringRef name)
			{
				T* retval = nullptr;
				BaseFactory<T>* factory = (*fDatabase)[name];
				if (factory)
				{
					retval = factory->Create();
				}
				return retval;
			}



			
		protected:
			static int fCount;
			static HashMap<String, BaseFactory<T>* >* fDatabase;
			
		private:
			

			
		};

	} //dawcomponents
} //microbees

#endif  // FACTORYDATABASE_H_INCLUDED
