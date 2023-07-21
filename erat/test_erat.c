#include "erat.h"

#include <stdio.h>          // printf
#include <stdlib.h>         // free

#undef NDEBUG
#include <assert.h>         // assert


#define SECTION( str, block ) \
    { printf("%s\n", #str ); { block }; printf("%s done\n\n", #str ); }


void msg_factors( ull* factors, ull count )
{
    printf( "invalid factors [ " );

    for ( ull i = 0; i < count; ++i )
        printf( "%lu ", factors[ i ] );

    printf( "]\n" );
}


int main()
{
    assert( pi( 15485864 ) == 1000000 );

    SECTION( nth_prime,
    {
        assert( nth_prime( 1 ) == 2 );
        assert( nth_prime( 2 ) == 3 );
        assert( nth_prime( 3 ) == 5 );
        assert( nth_prime( 4 ) == 7 );

        assert( nth_prime( 1000000 ) == 15485863 );
        assert( nth_prime( 1000001 ) == 15485867 );

        assert( nth_prime( 12000001 ) == 217645199 );
        assert( nth_prime( 13000000 ) == 236887691 );
    } );

    SECTION( nth_composite,
    {
        assert( nth_composite( 1 ) == 4 );
        assert( nth_composite( 2 ) == 6 );
        assert( nth_composite( 10 ) == 18 );

        for ( int i = 1; i < 10000; ++i )
        {
            ull n = nth_composite( i );
            ull primes = pi( n );
            if ( i + 1 + primes != n )
                printf( "i=%d n=%lu pi(n)=%lu\n", i, n, primes );
            assert( i + 1 + primes == n );
        }
    } );

    SECTION( consistency,
    {
        for ( int i = 1; i < 10000; ++i )
        {
            ull n = nth_composite( i );

            ull* factors = 0;
            ull count = 0;
            prime_factors( n, &factors, &count );

            if ( count <= 1 )
                msg_factors( factors, count );
            assert( count > 1 );

            ull prod = 1;
            for ( ull f = 0; f < count; ++f )
                prod *= factors[ f ];

            if ( n != prod ) msg_factors( factors, count );
            assert( n == prod );

            free( factors );
        }
    } );

    return 0;
}
