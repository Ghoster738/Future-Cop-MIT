cmake_minimum_required(VERSION 3.10)

# Enable Testing
enable_testing()

# Test Ray Code
add_executable(ray_test Utilities/Collision/Ray.cpp)
target_link_libraries(ray_test PRIVATE FC_IFF_IO)
add_test( NAME ray_test COMMAND $<TARGET_FILE:ray_test> )

# Test Plane Code
add_executable(plane_test Utilities/Collision/Plane.cpp)
target_link_libraries(plane_test PRIVATE FC_IFF_IO)
add_test( NAME plane_test COMMAND $<TARGET_FILE:plane_test> )

# Test Triangle Code
add_executable(triangle_test Utilities/Collision/Triangle.cpp)
target_link_libraries(triangle_test PRIVATE FC_IFF_IO)
add_test( NAME triangle_test COMMAND $<TARGET_FILE:triangle_test> )

# Test Math Code
add_executable(math_test Utilities/Math.cpp)
target_link_libraries(math_test PRIVATE FC_IFF_IO)
add_test( NAME math_test COMMAND $<TARGET_FILE:math_test> )