#include "MRParallelFor.h"

namespace MR
{

void parallelFor( size_t begin, size_t end, FunctionRef<void ( size_t )> f )
{
    tbb::parallel_for( tbb::blocked_range( begin, end ), [&f] ( const tbb::blocked_range<size_t>& range )
    {
        for ( auto i = range.begin(); i != range.end(); ++i )
            f( i );
    } );
}

void parallelFor( size_t begin, size_t end, FunctionRef<void ( void*, size_t )> f, FunctionRef<void* ()> ctx )
{
    tbb::parallel_for( tbb::blocked_range( begin, end ), [&f, &ctx] ( const tbb::blocked_range<size_t>& range )
    {
        void* ctx_ = ctx();
        for ( auto i = range.begin(); i != range.end(); ++i )
            f( ctx_, i );
    } );
}

} // namespace MR
