
#include "erat.h"

#include <stdio.h>          // printf
#include <stdint.h>         // uint64_t
#include <stdlib.h>         // strtoul, exit
#include <string.h>         // strcmp
#include <math.h>           // log1pl


void usage( char* arg )
{
    fprintf( stderr, "Usage: %s <prime | factor | composite | pi> N\n", arg );
}


int main( int argc, char** argv )
{
    if ( argc < 3 )
        usage( argv[ 0 ] ), exit( 1 );

    uint64_t n = strtoul( argv[ 2 ], NULL, 0 );

    const char* arg = argv[ 1 ];

    if ( strcmp( arg, "prime" ) == 0 )
    {
        printf( "%lu\n", nth_prime( n ) );
        return 0;
    }

    if ( strcmp( arg, "factor" ) == 0 )
    {
        ull* factors = 0;
        ull count = 0;
        prime_factors( n, &factors, &count );

        for ( ull i = 0; i < count; ++i )
            printf( "%s%lu", i == 0 ? "" : " ", factors[ i ] );
        printf( "\n" );

        free( factors );

        return 0;
    }

    if ( strcmp( arg, "composite" ) == 0 )
    {
        printf( "%lu\n", nth_composite( n ) );
        return 0;
    }

    if ( strcmp( arg, "pi" ) == 0 )
    {
        printf( "%lu\n", pi( n ) );
        return 0;
    }

    usage( argv[ 0 ] );
    return 1;
}
