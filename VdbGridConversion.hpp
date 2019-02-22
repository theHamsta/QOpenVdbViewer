/*
 * VdbGridConversion.hpp
 * Copyright (C) 2019 seitz_local <seitz_local@lmeXX>
 *
 * Distributed under terms of the GPLv3 license.
 */

#pragma once

#ifndef Q_MOC_RUN
#    pragma push_macro("Q_FOREACH")
#    pragma push_macro("foreach")
#    pragma push_macro("slots")
#    undef Q_FOREACH
#    undef foreach
#    undef slots
#    include <openvdb/openvdb.h>
#    include <openvdb/tools/ValueTransformer.h>
#    pragma pop_macro("Q_FOREACH")
#    pragma pop_macro("foreach")
#    pragma pop_macro("slots")
#endif

namespace internal
{
template< class FromGrid_T, class ToGrid_T >
struct ConversionOp
{
    static void op(const typename FromGrid_T::ValueAllCIter& iter, typename ToGrid_T::Accessor& accessor)
    {
        auto casted = static_cast< typename ToGrid_T::ValueType >(*iter);
        if (iter.isVoxelValue())
        { // set a single voxel
            accessor.setValue(iter.getCoord(), casted);
        }
        else
        { // fill an entire tile
            openvdb::CoordBBox bbox;
            iter.getBoundingBox(bbox);
            accessor.getTree()->fill(bbox, casted);
        }
    }
};
} // namespace internal

template< typename OutGrid_T >
typename OutGrid_T::Ptr convertVdbGrid(openvdb::GridBase::Ptr grid)
{
    typename OutGrid_T::Ptr outGrid = OutGrid_T::create();
    using namespace openvdb::tools;
    using namespace openvdb;

    if (grid->isType< BoolGrid >())
    {
        auto castedGrid = gridPtrCast< BoolGrid >(grid);
        auto accessor   = outGrid->getAccessor();
        transformValues(castedGrid->cbeginValueAll(), accessor, internal::ConversionOp< BoolGrid, OutGrid_T >::op);
    }
    else if (grid->isType< FloatGrid >())
    {
        auto castedGrid = gridPtrCast< FloatGrid >(grid);
        auto accessor   = outGrid->getAccessor();
        transformValues(castedGrid->cbeginValueAll(), accessor, internal::ConversionOp< FloatGrid, OutGrid_T >::op);
    }
    else if (grid->isType< DoubleGrid >())
    {
        auto castedGrid = gridPtrCast< DoubleGrid >(grid);
        auto accessor   = outGrid->getAccessor();
        transformValues(castedGrid->cbeginValueAll(), accessor, internal::ConversionOp< DoubleGrid, OutGrid_T >::op);
    }
    else if (grid->isType< Int32Grid >())
    {
        auto castedGrid = gridPtrCast< Int32Grid >(grid);
        auto accessor   = outGrid->getAccessor();
        transformValues(castedGrid->cbeginValueAll(), accessor, internal::ConversionOp< Int32Grid, OutGrid_T >::op);
    }
    else if (grid->isType< Int64Grid >())
    {
        auto castedGrid = gridPtrCast< Int64Grid >(grid);
        auto accessor   = outGrid->getAccessor();
        transformValues(castedGrid->cbeginValueAll(), accessor, internal::ConversionOp< Int64Grid, OutGrid_T >::op);
    }
    return outGrid;
}
