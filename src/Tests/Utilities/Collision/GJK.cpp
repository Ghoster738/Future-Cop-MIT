#include "../../../Utilities/Collision/GJK.h"
#include "../../../Utilities/Collision/GJKPolyhedron.h"
#include "../../../Utilities/Random.h" // Used in the stress test.

#include <glm/common.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <random> // Used in the stress test using the Random device.
#include <thread> // Definitely used in this stress test.
#include <ctime>

#include "Helper.h"

namespace  {
    const int FAILURE = 1;
    const int SUCCESS = 0;
}

using Utilities::Collision::GJKPolyhedron;
using Utilities::Collision::GJKShape;
using Utilities::Collision::GJK;
using Utilities::Random; // Used in the stress test.

std::vector<glm::vec3> generateCubeData( glm::vec3 scale, glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> cube_data;

    cube_data.push_back( glm::vec3( -scale.x, -scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x, -scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x,  scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x,  scale.y, -scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x, -scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x, -scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3( -scale.x,  scale.y,  scale.z ) + center );
    cube_data.push_back( glm::vec3(  scale.x,  scale.y,  scale.z ) + center );

    return cube_data;
}

std::vector<glm::vec3> generateTetrahedronData( glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> poly_data;

    poly_data.push_back( glm::vec3(  0.94281,  0.35355, -1.0/3.0 ) + center );
    poly_data.push_back( glm::vec3( -0.47140,  0.69865, -1.0/3.0 ) + center );
    poly_data.push_back( glm::vec3( -0.47140, -0.93435, -1.0/3.0 ) + center );
    poly_data.push_back( glm::vec3(  0.00000, -0.11785,  1.00000 ) + center );

    return poly_data;
}

std::vector<glm::vec3> generateTriangleData( glm::vec3 center = glm::vec3(0,0,0) ) {
    std::vector<glm::vec3> poly_data;
    
    center -= glm::vec3(-0.166667, 0, 0);

    poly_data.push_back( glm::vec3(  0.5,  0.0, 0 ) + center );
    poly_data.push_back( glm::vec3( -0.5,  0.5, 0 ) + center );
    poly_data.push_back( glm::vec3( -0.5, -0.5, 0 ) + center );

    return poly_data;
}

int outsideTest( int x, int y, int z, const GJKShape &shape, std::string name, glm::vec3 displacement = glm::vec3(0) ) {
    int status = SUCCESS;
    glm::vec3 position = 8.0f * glm::vec3( x, y, z );

    GJKPolyhedron cube_outside( generateCubeData( glm::vec3(1,1,1), position + displacement ) );
    GJKPolyhedron tetrahedron_outside( generateTetrahedronData( position + displacement ) );
    GJKPolyhedron triangle_outside( generateTriangleData( position + displacement ) );

    if( GJK::hasCollision(shape, cube_outside) ) {
        std::cout << "Cube did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        displayVec3( "d", displacement, std::cout );
        status = FAILURE;
    }
    if( GJK::hasCollision(shape, tetrahedron_outside) ) {
        std::cout << "Tetrahedron did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        displayVec3( "d", displacement, std::cout );
        status = FAILURE;
    }
    if( GJK::hasCollision(shape, triangle_outside) ) {
        std::cout << "Triangle did collide outside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        displayVec3( "d", displacement, std::cout );
        status = FAILURE;
    }

    return status;
}

int insideTest( int x, int y, int z, const GJKShape &shape, std::string name, glm::vec3 displacement = glm::vec3(0) ) {
    int status = SUCCESS;
    glm::vec3 position = 0.5f * glm::vec3( x, y, z );

    GJKPolyhedron cube_inside( generateCubeData( glm::vec3(1,1,1), position + displacement ) );
    GJKPolyhedron tetrahedron_inside( generateTetrahedronData( position + displacement ) );
    GJKPolyhedron triangle_inside( generateTriangleData( position + displacement ) );

    if( !GJK::hasCollision(shape, cube_inside) ) {
        std::cout << "Cube did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        displayVec3( "d", displacement, std::cout );
        status = FAILURE;
    }

    if( !GJK::hasCollision(shape, tetrahedron_inside) ) {
        std::cout << "Tetrahedron did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        displayVec3( "d", displacement, std::cout );
        status = FAILURE;
    }

    if( !GJK::hasCollision(shape, triangle_inside) ) {
        std::cout << "Triangle did not collide inside " << name << std::endl;
        displayVec3( "v", position, std::cout );
        displayVec3( "d", displacement, std::cout );
        status = FAILURE;
    }

    return status;
}

int quaderentTest( int x, int y, int z ) {
    int status = SUCCESS;
    glm::vec3 position = 16.0f * glm::vec3( x, y, z );

    GJKPolyhedron cube( generateCubeData( glm::vec3(1,1,1), position ) );

    for( int x = -1; x <= 1 && status == SUCCESS; x++) {
        for( int y = -1; y <= 1 && status == SUCCESS; y++) {
            for( int z = -1; z <= 1 && status == SUCCESS; z++) {
                status |= insideTest(x, y, z, cube, "cube", position);
                if( !(x == y && y == z) ) {
                    status |= outsideTest(x, y, z, cube, "cube", position);
                }
            }
        }
    }

    return status;
}

std::mutex say_guard;
std::random_device random_device;
std::mutex random_device_guard;

void sayProblem( int width, int height, glm::vec3 position_of_camera, glm::vec2 rotation, float distance_away ) {
    say_guard.lock();

    std::cout << "This GJK algorithm broke at\n";
    std::cout << "  width = " << width << "\n";
    std::cout << "  height = " << height << "\n";
    std::cout << "  position = ( " << position_of_camera.x << ", " << position_of_camera.y << ", " << position_of_camera.z << " );\n";
    std::cout << "  rotation = ( " << rotation.x << ", " << rotation.y << " );\n";
    std::cout << "  distance_away = " << distance_away << std::endl;

    say_guard.unlock();
}

int stressTest( Random::Generator &general ) {
    int status = SUCCESS;

    std::vector<glm::vec3> camera_data(8);

    camera_data[0] = glm::vec3( 127.6480636596679688, 3.1758630275726318, 60.8731765747070312 );
    camera_data[1] = glm::vec3( 127.5414276123046875, 3.1758630275726318, 60.7715873718261719 );
    camera_data[2] = glm::vec3( 127.6340255737304688, 3.2561616897583008, 60.8879165649414062 );
    camera_data[3] = glm::vec3( 127.5273895263671875, 3.2561612129211426, 60.7863273620605469 );
    camera_data[4] = glm::vec3( 114.6178359985351562, -126.2063064575195312, 287.9737854003906250 );
    camera_data[5] = glm::vec3( -98.6433944702148438, -126.2067489624023438, 84.7968063354492188 );
    camera_data[6] = glm::vec3( 86.5334472656250000, 34.3913993835449219, 317.4512329101562500 );
    camera_data[7] = glm::vec3( -126.7258300781250000, 34.3909606933593750, 114.2761077880859375 );

    int width;
    int height;
    glm::vec3 position_of_camera;
    glm::vec2 rotation;
    float distance_away;

    /*
    glm::mat4 projection_matrix;
    glm::mat4 extra_matrix_0;
    glm::mat4 extra_matrix_1;
    glm::mat4 extra_matrix_2;

    GJKPolyhedron cube_shape( camera_data );

    width = general.nextUnsignedInt() % 3520 + 320;
    height = general.nextUnsignedInt() % 1808 + 240;
    position_of_camera.x = general.nextFloat( 14 * 16, 0 );
    position_of_camera.y = 0;
    position_of_camera.z = general.nextFloat( 16 * 16, 0 );
    rotation.x = general.nextFloat( 7, 0 );
    rotation.y = general.nextFloat( 7, 0 );
    distance_away = general.nextFloat( 100, -100 );

    projection_matrix = glm::perspective( glm::pi<float>() / 4.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 200.0f );

    extra_matrix_0 = glm::translate( glm::mat4(1.0f), glm::vec3( 0, 0, distance_away ) );
    extra_matrix_1 = glm::rotate( glm::mat4(1.0f), rotation.y, glm::vec3( 1.0, 0.0, 0.0 ) ); // rotate up and down.
    extra_matrix_2 = extra_matrix_0 * extra_matrix_1;
    extra_matrix_1 = glm::rotate( glm::mat4(1.0f), rotation.x, glm::vec3( 0.0, 1.0, 0.0 ) ); // rotate left and right.
    extra_matrix_0 = extra_matrix_2 * extra_matrix_1;
    extra_matrix_1 = glm::translate( glm::mat4(1.0f), -position_of_camera );
    extra_matrix_2 = extra_matrix_0 * extra_matrix_1;

    glm::mat4 inverse = glm::inverse( projection_matrix * extra_matrix_2 );*/
    auto camera_shape = GJKPolyhedron( camera_data );

    for( int x = 0; x < 14 && status != FAILURE; x++ ) {
        for( int y = 0; y < 16 && status != FAILURE; y++ ) {
            std::vector<glm::vec3> section_data( 8, glm::vec3() );

            glm::vec3 min(     x * 16,  6,     y * 16 );
            glm::vec3 max( min.x + 16, -6, min.y + 16 );

            section_data[0] = glm::vec3( min.x, min.y, min.z );
            section_data[1] = glm::vec3( max.x, min.y, min.z );
            section_data[2] = glm::vec3( min.x, max.y, min.z );
            section_data[3] = glm::vec3( max.x, max.y, min.z );
            section_data[4] = glm::vec3( min.x, min.y, max.z );
            section_data[5] = glm::vec3( max.x, min.y, max.z );
            section_data[6] = glm::vec3( min.x, max.y, max.z );
            section_data[7] = glm::vec3( max.x, max.y, max.z );

            GJKPolyhedron section_shape( section_data );

            size_t limit = 128;

            // Its status is not important. Just detect if it got stuck.
            GJK::hasCollision(camera_shape, section_shape, limit);

            if( limit == 0 ) {
                sayProblem( width, height, position_of_camera, rotation, distance_away );
                status = FAILURE;
            }
        }
    }

    return status;
}

void stressThread( int &global_status, std::mutex &status_guard, uint16_t running_minutes, uint64_t *times_run_r = nullptr ) {
    int status = SUCCESS;
    Random random;
    std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::minutes;

    const auto starting_time = high_resolution_clock::now();
    auto current_time = starting_time;

    while( duration_cast<minutes>(current_time - starting_time).count() <= running_minutes && status != FAILURE ) {
        random_device_guard.lock();

        random.setSeeder( dist(random_device) );

        random_device_guard.unlock();

        Random::Generator general = random.getGenerator();

        size_t RUN_CYCLE = 0xFFF;

        for( size_t times = 0; times < RUN_CYCLE && status != FAILURE; times++ ) {
            status |= stressTest( general );
        }

        if( times_run_r != nullptr && *times_run_r != std::numeric_limits<uint64_t>::max() ) {
            uint64_t previous = *times_run_r;

            *times_run_r += RUN_CYCLE;

            if( previous > *times_run_r )
                *times_run_r = std::numeric_limits<uint64_t>::max();
        }

        current_time = high_resolution_clock::now();
    }

    if( status == FAILURE ) {
        status_guard.lock();

        global_status = FAILURE;

        status_guard.unlock();
    }
}

int main( int argc, char* argv[] ) {
    int status = SUCCESS;

    std::string input;

    if( argc > 1 ) {
        input = std::string( argv[1] );

        if( input != "--stress" )
            std::cout << "Use --stress to stress test the GJK algorithm rather than the usual routine of the test" << std::endl;
    }

    // Stress Test Do Not Enable this by Default.
    if( input == "--stress" ) {
        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::minutes;

        unsigned number_of_threads = 0;
        std::mutex status_guard;

        std::cout << "How Many threads do you want for this stress test. Enter 0 for all that is available in the system." << std::endl;

        std::cout << "Threads: ";
        std::cin >> number_of_threads;

        if( number_of_threads == 0 )
            number_of_threads = std::thread::hardware_concurrency();

        std::vector<std::thread> threads;
        std::vector<uint64_t> map_views(number_of_threads, 0);

        int tminutes = 2;

        std::cout << "\nHow many minutes do you want this test to run on your computer. Note: The worst cause for speed is that GJK never halts. Warning: If you do not want to run this stress test then you can simply enter 0 in the minutes and this program will exit." << std::endl;

        std::cout << "Minutes: ";
        std::cin >> tminutes;

        const auto starting_time = high_resolution_clock::now();

        const auto system_time = std::chrono::system_clock::to_time_t(starting_time);
        std::cout << "Started test at " << std::ctime(&system_time) << std::endl;

        if( tminutes == 0 ) {
            return 0; // Exit the program.
        }

        for( size_t i = 0; i < number_of_threads; i++ ) {
            threads.push_back( std::thread( [&status, &status_guard, &map_views, tminutes, i]
                {
                    stressThread( status, status_guard, tminutes, &map_views[ i ] );
                }
            ) );
        }

        uint64_t total_times_on_threads = 0;

        for( size_t i = 0; i < number_of_threads; i++ ) {
            threads[i].join();
            say_guard.lock();
            std::cout << "Times run on thread " << i << " is " << map_views[ i ] << std::endl;
            say_guard.unlock();
            total_times_on_threads += map_views[ i ];
        }

        say_guard.lock();
        std::cout << "Total Times Run on Thread = " << total_times_on_threads << std::endl;

        const auto current_time = high_resolution_clock::now();
        const auto min_ran = duration_cast<minutes>(current_time - starting_time).count();

        if( min_ran != 0 ) {
            std::cout << "Minutes run = " << min_ran << std::endl;

            const auto runs_per_minute = total_times_on_threads / min_ran;

            std::cout << "Runs per minute = " << runs_per_minute << std::endl;

            const double runs_per_second = static_cast<double>(runs_per_minute) / 60.0;

            std::cout << "Runs per second = " << runs_per_second << std::endl;

            const auto end_system_time = std::chrono::system_clock::to_time_t(current_time);
            std::cout << "Ended test at " << std::ctime(&end_system_time) << std::endl;
        }
        say_guard.unlock();
    }

    // This is very slow, but it would test every quaderent.
    // As it is it will run insideTest and outsideTest about 729 times.
    for( int x = -1; x <= 1 && status == SUCCESS; x++) {
        for( int y = -1; y <= 1 && status == SUCCESS; y++) {
            for( int z = -1; z <= 1 && status == SUCCESS; z++) {
                status |= quaderentTest(x, y, z);
            }
        }
    }

    return status;
}
