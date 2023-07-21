
#pragma once


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
