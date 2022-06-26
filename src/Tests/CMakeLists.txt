cmake_minimum_required(VERSION 3.10)

# Enable Testing
enable_testing()

# Test Ray Code
add_executable(ray_test Utilities/Collision/Ray.cpp)
target_link_libraries(ray_test PRIVATE FC_IFF_IO)

# Define tests
add_test( NAME ray_test COMMAND $<TARGET_FILE:ray_test> )

# Test Ray Code
add_executable(plane_test Utilities/Collision/Plane.cpp)
target_link_libraries(plane_test PRIVATE FC_IFF_IO)

# Define tests
add_test( NAME plane_test COMMAND $<TARGET_FILE:plane_test> )