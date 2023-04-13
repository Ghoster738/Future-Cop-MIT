#include "../../Utilities/Random.h"
#include <iostream>
#include <string>
#include <vector>

using Utilities::Random;

namespace  {
    const int FAILURE = 1;
    const int SUCCESS = 0;

    std::vector<uint32_t> getNumbers( Random::Generator &gen, uint32_t number ) {
        std::vector<uint32_t> array;

        for( uint32_t state = 0; state < number; state++ ) {
            array.push_back( gen.nextUnsignedInt() );
        }

        return array;
    }

    std::vector<int32_t> getSNumbers( Random::Generator &gen, uint32_t number ) {
        std::vector<int32_t> array;

        for( uint32_t state = 0; state < number; state++ ) {
            array.push_back( gen.nextSignedInt() );
        }

        return array;
    }

    bool compareVectors( const std::vector<uint32_t> &expected, const std::vector<uint32_t> &result, std::string name, std::ostream *output_r ) {
        if( expected.size() != result.size() ) {
            if( output_r != nullptr )
                *output_r << "Error: " << name << " does not have the correct length. Expected: " << expected.size() << " Result: " << result.size() << std::endl;

            return false;
        }

        for( uint32_t i = 0; i < expected.size(); i++ ) {
            if( expected[i] != result[i] ) {
                if( output_r != nullptr )
                    *output_r << "Error: " << name << " at index " << i << ". Expected: 0x" << std::hex << expected[i] << " Result: 0x" << result[i] << std::dec << std::endl;

                return false;
            }
        }

        return true;
    }
    bool compareSVectors( const std::vector<int32_t> &expected, const std::vector<int32_t> &result, std::string name, std::ostream *output_r ) {
        if( expected.size() != result.size() ) {
            if( output_r != nullptr )
                *output_r << "Error Signed: " << name << " does not have the correct length. Expected: " << expected.size() << " Result: " << result.size() << std::endl;

            return false;
        }

        for( uint32_t i = 0; i < expected.size(); i++ ) {
            if( expected[i] != result[i] ) {
                if( output_r != nullptr )
                    *output_r << "Error Signed: " << name << " at index " << i << ". Expected: " << expected[i] << " Result: " << result[i] << std::endl;

                return false;
            }
        }

        return true;
    }
    bool compareFloatMaxMin( Random::Generator generator, uint32_t number_attempts, float max, float min, float margin_of_error, std::string name, std::ostream *output_r ) {
        float max_found = -std::numeric_limits<float>::max();
        float min_found =  std::numeric_limits<float>::max();
        float current_value;
        bool fatal_error = false;

        for( uint32_t i = 0; i < number_attempts; i++ ) {
            current_value = generator.nextFloat( max, min );

            if( current_value > max && current_value < min ) {
                if( output_r != nullptr )
                    *output_r << "Error Float: " << name << " at index " << i << " which is " << current_value << " is not inside [" << min << ", " << max << "]" << std::endl;
                return false;
            }
            max_found = std::max(max_found, current_value);
            min_found = std::min(min_found, current_value);
        }

        if( max_found < max - margin_of_error ) {
            if( output_r != nullptr )
                *output_r << "Error Float: " << name << " has max of " << max_found << ", but it is not close (" << margin_of_error << ") enough to " << max << std::endl;
            fatal_error = true;
        }
        if( min_found > min + margin_of_error ) {
            if( output_r != nullptr )
                *output_r << "Error Float: " << name << " has min of " << min_found << ", but it is not close (" << margin_of_error << ") enough to " << min << std::endl;
            fatal_error = true;
        }
        return !fatal_error;
    }
}

int main() {
    int problem = SUCCESS;

    { // General Test.
        Random random( 0x5F93282D6FDEC ); // That is a time stamp.
        Random::Generator general = random.getGenerator();

        // This test is to detect if there is a change in the random number algorithm.
        std::vector<uint32_t> general_numbers = getNumbers( general, 128 );
        std::vector<uint32_t> numbers = {0x614f2f30, 0x8773d036, 0x31dc9bd2, 0x7b5d2522, 0xe6121980, 0x6a2c7668, 0x1895b494, 0xe729ce2a, 0xf62b59c1, 0xbc5364e8, 0xc7edb20f, 0xb89828c9, 0x2bcffb3c, 0xb550b286, 0x63f9369e, 0xb4ceb7d7, 0x4c4c1dbf, 0x92126731, 0x76304ad0, 0x3f68263f, 0x2abe166c, 0xdf334806, 0xa016e88f, 0x599b0637, 0x1f54cfa7, 0xdcc31d2b, 0xa80f0345, 0x7679fb6e, 0xa2dc9656, 0x67b936f9, 0xe1365607, 0xd632719f, 0x1ec2370f, 0xacbd43bb, 0x5f93333c, 0xdf2f8cb8, 0xbbb1902b, 0x86b60ca6, 0x6a81ff0a, 0x13ab76f5, 0x1bed2eb9, 0x85788674, 0xd596ef0d, 0x7c0e7a4e, 0xd934f93b, 0x8d01684a, 0x2c47176, 0xb6467b34, 0x6c25e8b9, 0x8945ff4c, 0xc605088b, 0x9e0bc5be, 0x638ce0e7, 0xc1520d51, 0xa3e9c2c3, 0x214f8940, 0xadf9d6de, 0x43ba1ca6, 0xc49249c9, 0x5f330b88, 0x8c2b13f0, 0x7a4867f8, 0x25ef1e8b, 0xfb959c2a, 0xefa78305, 0xa25b9492, 0x5ebad921, 0xc08f1de, 0x4e9569d3, 0x249f05b3, 0x41d81cf0, 0x9e4ba338, 0xfab7d225, 0xd6a2eaac, 0xe1e222f8, 0xdc7d0000, 0x1cb997b7, 0xd0431612, 0x4a673874, 0x32f0478b, 0xaba5b9cd, 0x54fb3960, 0x909fc03c, 0x29252a56, 0x16e6992d, 0x3836dce0, 0xfe064d95, 0xfd02e13, 0x6c8c778, 0x106f6f7f, 0xea3d29b5, 0xd2df3a78, 0xcb29709f, 0xf3a0e9b7, 0x81e05c06, 0xa4dcfb18, 0xd4d88658, 0xac8349e3, 0x4bbbc7d6, 0xd1f33ed, 0xacde92c5, 0xc618f22d, 0x3edca513, 0x52ba9729, 0xa90b438e, 0x2cc270a7, 0xd6f76a9b, 0xc6285ea0, 0xca9aa0f7, 0xa8d8fc44, 0x805cf40e, 0x3033129c, 0xe4f14b41, 0x2f06a25b, 0xcba74bcd, 0xc259944a, 0xadc24fa9, 0x7131b9b, 0x31eac567, 0x377c4627, 0x6c49d35c, 0x5335f0ee, 0x95ff5fdf, 0x50c88dde, 0x4dab1ce9, 0xf43b705e, 0x80447d68, 0x2aaad92c};

        if( !compareVectors( general_numbers, numbers, "General Test", &std::cout ) )
            problem = FAILURE;

        std::vector<int32_t> general_signed_numbers = getSNumbers( general, 4 );
        std::vector<int32_t> signed_numbers = {1816026831, 1700756648, 744911551, -137246805};

        if( !compareSVectors( general_signed_numbers, signed_numbers, "General Signed Test", &std::cout ) )
            problem = FAILURE;

        if( !compareFloatMaxMin( general, 128, 24.0, -32.0, 0.4375, "Float Min Max", &std::cout ) )
            problem = FAILURE;
    }

    { // Initialization Test.

        // Setup these three random number generator causes.
        // They should all match the output of the original.
        Random random;
        Random::Generator first = random.getGenerator();
        Random::Generator second = random.getGenerator();

        std::vector<uint32_t> first_array  = getNumbers(  first, 64 );
        std::vector<uint32_t> second_array = getNumbers( second, 64 );

        Random random_z(0);
        Random::Generator first_z = random_z.getGenerator();
        Random::Generator second_z = random_z.getGenerator();

        std::vector<uint32_t> first_array_z  = getNumbers( first_z,  64 );
        std::vector<uint32_t> second_array_z = getNumbers( second_z, 64 );

        Random random_1(1);
        Random::Generator first_1 = random_1.getGenerator();
        Random::Generator second_1 = random_1.getGenerator();

        std::vector<uint32_t> first_array_1  = getNumbers( first_1,  64 );
        std::vector<uint32_t> second_array_1 = getNumbers( second_1, 64 );

        if( compareVectors( first_array, second_array, "", nullptr ) ) {
            std::cout << "The first array should not match the second array!" << std::endl;
            problem = FAILURE;
        }
        if( !compareVectors( first_array, first_array_z, "Init Zero Test 1", &std::cout ) )
            problem = FAILURE;
        if( !compareVectors( second_array, second_array_z, "Init Zero Test 2", &std::cout ) )
            problem = FAILURE;
        if( !compareVectors( first_array, first_array_1, "Init  One Test 1", &std::cout ) )
            problem = FAILURE;
        if( !compareVectors( second_array, second_array_1, "Init One Test 2", &std::cout ) )
            problem = FAILURE;
    }

    return problem;
}
