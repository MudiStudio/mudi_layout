#ifndef BASEFACTORYDATABASE_H_INCLUDED
#define BASEFACTORYDATABASE_H_INCLUDED

namespace mudi
{
    namespace mudi_layout
    {
        using namespace juce;

        using BaseFactoryDb = HashMap<String, BaseFactory<BaseLayoutComponent>*>;
        using BaseFactoryDatabase = FactoryDatabase<BaseLayoutComponent>;

        
        BaseFactoryDatabase& InitBaseFactoryDatabase();

    } //
} //

#endif  // BASEFACTORYDATABASE_H_INCLUDED
