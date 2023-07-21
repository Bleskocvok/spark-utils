
#include <stdlib.h>         // calloc, free, NULL, realloc, exit
#include <math.h>           // log1pl
#include <stdint.h>         // uint64_t


typedef  uint64_t  ull;
typedef  unsigned char  byte_t;

typedef void ( * callback_t )      ( ull n, void* ctx );
typedef void ( * jump_callback_t ) ( ull n, ull divisor, void* ctx );


int sieve_init( byte_t** ptr, ull max );

void sieve_free( byte_t* sieve );

void sieve_walk( byte_t* sieve, ull max,
                 jump_callback_t jump, callback_t prime, void* ctx );

void quick_walk( ull max, jump_callback_t jump, callback_t prime, void* ctx );

void quick_walk_composite( ull max, callback_t prime, void* ctx );

void prime_factors( ull n, ull** out, ull* count );

void nth_callback( ull n, void* ctx );

ull nth_prime( ull n );

ull nth_composite( ull n );

ull pi( ull n );

ull pi_n_upper_bound( ull n );


ull pi_n_upper_bound( ull n )
{
    if ( n >= 7022 )
        return n * log1pl( n )
             + n * ( log1pl( log1pl( n ) ) - 0.9385 );

    return n * log1pl( n )
         + n * log1pl( log1pl( n ) );
}


typedef struct
{
    ull count;
    ull sought;
    ull n;

} counter_t;


void nth_callback( ull n, void* ctx )
{
    counter_t* cter = (counter_t*) ctx;

    if ( ++cter->count == cter->sought )
        cter->n = n;
}


ull nth_prime( ull n )
{
    if ( n == 0 )
        return 0;

    if ( n == 1 )
        return 2;

    ull top_bound = pi_n_upper_bound( n );

    counter_t cter;
    cter.count = 0;
    cter.sought = n;
    cter.n = 0;

    quick_walk( top_bound, NULL, nth_callback, &cter );

    return cter.n;
}


ull pi( ull n )
{
    counter_t cter;
    cter.count = 0;
    cter.sought = -1;
    cter.n = 0;

    quick_walk( n, NULL, nth_callback, &cter );

    return cter.count;
}


ull nth_composite( ull n )
{
    ull top_bound = 4 + pi_n_upper_bound( n );

    counter_t cter;
    cter.count = 0;
    cter.sought = n;
    cter.n = 0;

    quick_walk_composite( top_bound, nth_callback, &cter );

    return cter.n;
}


typedef struct
{
    ull n;
    ull cap;
    ull size;
    ull* factors;

} factor_ctx_t;


void add_factor( ull n, void* ctx )
{
    factor_ctx_t* f = (factor_ctx_t*) ctx;

    if ( f->size == f->cap )
    {
        f->cap *= 2;
        f->factors = realloc( f->factors, f->cap * sizeof( ull ) );
        if ( !f->factors )
            exit( 1 );
    }

    f->factors[ f->size ] = n;
    ++f->size;
}


void factor( ull p, void* ctx )
{
    factor_ctx_t* f = (factor_ctx_t*) ctx;

    while ( f->n > 1 && f->n % p == 0 )
    {
        add_factor( p, ctx );
        f->n /= p;
    }
}


void prime_factors( ull n, ull** out, ull* count )
{
    factor_ctx_t ctx;
    ctx.cap = 1;
    ctx.size = 0;
    ctx.n = n;

    ctx.factors = calloc( ctx.cap, sizeof( ull ) );
    if ( !ctx.factors )
        exit( 1 );

    quick_walk( sqrt( n ), NULL, factor, &ctx );

    if ( ctx.size == 0 || ctx.n > 1 )
        add_factor( ctx.n, &ctx );

    *out = ctx.factors;
    *count = ctx.size;
}


void sieve_mark( byte_t* sieve, ull n )
{
    sieve[ n / 8 ] |= 0x1 << ( n & 0x7 );
}


int sieve_eight_marked( const byte_t* sieve, ull n )
{
    return sieve[ n / 8 ] == 0xFF;
}


int sieve_is_marked( const byte_t* sieve, ull n )
{
    return sieve[ n / 8 ] & ( 0x1 << ( n & 0x7 ) );
}


int sieve_init( byte_t** ptr, ull max )
{
    *ptr = calloc( ( max + 1 ) / 8 + 1, 1 );
    return !*ptr ? -1 : 0;
}


void sieve_free( byte_t* sieve )
{
    free( sieve );
}


void sieve_walk( byte_t* sieve, ull max,
                 jump_callback_t jump, callback_t prime, void* ctx )
{
    for ( ull i = 2; i <= max; ++i )
    {
        // if ( ( i & 0x7 ) == 0 && sieve_eight_marked( sieve, i ) )
        // {
        //     i += 7;
        //     continue;
        // }

        if ( sieve_is_marked( sieve, i ) )
            continue;

        if ( prime )
            prime( i, ctx );

        for ( ull j = i * 2; j <= max; j += i )
        {
            sieve_mark( sieve, j );
            if ( jump )
                jump( j, i, ctx );
        }

        // if ( jump )
        //     for ( ull j = i * 2; j <= max; j += i )
        //     {
        //         sieve_mark( sieve, j );
        //         if ( jump )
        //             jump( j, i, ctx );
        //     }
        // else
        //     for ( ull j = i * 2; j <= max; j += i )
        //         sieve_mark( sieve, j );


        // ull j;
        // for ( j = i * 2; j + 4 * i < max; j += i * 4 )
        // {
        //     sieve_mark( sieve, j );
        //     sieve_mark( sieve, j + i );
        //     sieve_mark( sieve, j + 2 * i );
        //     sieve_mark( sieve, j + 3 * i );
        //     // if ( jump )
        //     // {
        //     //     jump( j, i, ctx );
        //     //     jump( j + i, i, ctx );
        //     //     jump( j + i * 2, i, ctx );
        //     //     jump( j + i * 3, i, ctx );
        //     // }
        // }
        // for ( ; j <= max; j += i )
        // {
        //     sieve_mark( sieve, j );
        //     // if ( jump )
        //     //     jump( j, i, ctx );
        // }
    }
}


void quick_walk( ull max, jump_callback_t jump, callback_t prime, void* ctx )
{
    byte_t* sieve = NULL;
    sieve_init( &sieve, max );
    sieve_walk( sieve, max, jump, prime, ctx );
    sieve_free( sieve );
}


void quick_walk_composite( ull max, callback_t compos, void* ctx )
{
    if ( !compos )
        return;

    byte_t* sieve = NULL;
    sieve_init( &sieve, max );
    sieve_walk( sieve, max, NULL, NULL, ctx );

    for ( ull i = 4; i <= max; ++i )
    {
        if ( sieve_is_marked( sieve, i ) )
            compos( i, ctx );
    }

    sieve_free( sieve );
}
