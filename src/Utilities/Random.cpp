#include "Random.h"

namespace {
    uint64_t M32 = UINT64_C(0x2545F4914F6CDD1D);

    uint64_t reverseBits( uint64_t n ) {
        uint64_t answer = 0;
        for( int i = 63; i >= 0; i-- ) {
            answer |= (n & 1) << i;
            n >>= 1;
        }

        return answer;
    }
}

namespace Utilities {

Random::Generator::Generator( uint64_t current_seed_param ) :
    current_seed( current_seed_param )
{}

void Random::Generator::nextSeed() {
    current_seed ^= current_seed >> 12;
    current_seed ^= current_seed << 25;
    current_seed ^= current_seed >> 27;
}

uint32_t Random::Generator::nextUnsignedInt() {
    nextSeed();
    return (current_seed * M32) >> 32;
}

int32_t Random::Generator::nextSignedInt() {
    nextSeed();
    return static_cast<int32_t>(nextUnsignedInt());
}

float Random::Generator::nextFloat( float max, float min ) {
    nextSeed();
    const uint64_t value = current_seed * M32;

    double far = static_cast<double>(std::numeric_limits<uint64_t>::max() - value) / static_cast<double>(std::numeric_limits<uint64_t>::max());
    double small = 1.0 - far;

    return far * max + small * min;
}

Random::Random( uint64_t current_seeder_param ) : current_seeder( current_seeder_param ) {
    if( current_seeder == 0 )
        current_seeder = 1;
}

Random::~Random() {
}

void Random::setSeeder( uint64_t seeder ) {
    if( seeder == 0 )
        seeder = 1;

    current_seeder = seeder;
}

Random::Generator Random::getGenerator() {
    auto generator = Generator( reverseBits( current_seeder ) );

    current_seeder++;
    if( current_seeder == 0 )
        current_seeder = 1;

    return generator;
}

}
