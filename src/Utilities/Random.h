#ifndef UTILITIES_RANDOM_H
#define UTILITIES_RANDOM_H

#include <stdint.h>
#include <mutex>

namespace Utilities {

/**
 * This is a factory of the random number generators.
 *
 * I know that C++ has its own random number solution, but I wanted to make this program determinisic, and for that I must know the exact algorithms for random numbers.
 *
 * Random creates the seeds for Generator using the recommendations from https://www.dice-research.org/news/2020-01-18-seedsatscale/.
 * Generator uses the XORShift64* generator. https://en.wikipedia.org/wiki/Xorshift
 *
 * This generator is to be used for things like choosing which path a tank should go to. Each entity
 * should have their own Generator. The Random class is thread safe, but not the
 * Generator.
 * @warning This is for gameplay uses only. DO NOT USE THIS FOR SECURITY. Using class for encryption or anything simualar would be rejected for merge until fixed with a cryptographically-secure random number generator.
 */
class Random {
public:
    /**
     * Unlike the main class, Random, this class is not thread safe.
     */
    class Generator {
    private:
        uint64_t current_seed;
    public:
        Generator( uint64_t current_seed );

        /**
         * Increment the seed for the RandomGenrator
         */
        void nextSeed();

        /**
         * @note The method calls nextSeed().
         * @return An unsigned int32_t pseudorandom number.
         */
        uint32_t nextUnsignedInt();
        /**
         * @note The method calls nextSeed().
         * @return A signed int32_t pseudorandom number.
         */
        int32_t nextSignedInt();
        /**
         * @note The method calls nextSeed().
         * @return A float between max and min.
         */
        float nextFloat( float max = 1.0, float min = 0.0 );
    };
private:
    uint64_t current_seeder;
    std::mutex guard;

public:
    Random( uint64_t current_seeder = 1 );
    virtual ~Random();

    /**
     * This sets the seeder to be some other value.
     * @note This is thread safe, but you would need to wait for a few microseconds maybe.
     * @param seeder A 64 bit value, preferably std::device or time, to set to be the base of the seeds.
     */
    void setSeeder( uint64_t seeder );

    /**
     * This gives out the random number generator.
     * @note This is thread safe, but you would need to wait for a few microseconds maybe.
     * @return The random number generator.
     */
    Generator getGenerator();
};

}

#endif // UTILITIES_RANDOM_H
