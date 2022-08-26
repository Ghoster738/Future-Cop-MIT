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

# Test GridReg2D Code
add_executable(grid_reg_2d_test Utilities/GridReg2D.cpp)
target_link_libraries(grid_reg_2d_test PRIVATE FC_IFF_IO)
add_test( NAME grid_reg_2d_test COMMAND $<TARGET_FILE:grid_reg_2d_test> )

# Test GridSq2D Code
add_executable(grid_sq_2d_test Utilities/GridSq2D.cpp)
target_link_libraries(grid_sq_2d_test PRIVATE FC_IFF_IO)
add_test( NAME grid_sq_2d_test COMMAND $<TARGET_FILE:grid_sq_2d_test> )

# Test PixelFormat Code
add_executable(pixel_format_test Utilities/PixelFormat)
target_link_libraries(pixel_format_test PRIVATE FC_IFF_IO)
add_test( NAME pixel_format_test COMMAND $<TARGET_FILE:pixel_format_test> )

# Test Model Builder Code
add_executable(model_builder_test Utilities/ModelBuilder.cpp)
target_link_libraries(model_builder_test PRIVATE FC_IFF_IO)
add_test( NAME model_builder_test COMMAND $<TARGET_FILE:model_builder_test> )

# Test TilResource Code
add_executable(til_resource_test Data/Mission/TilResource.cpp)
target_link_libraries(til_resource_test PRIVATE FC_IFF_IO)
add_test( NAME til_resource_test COMMAND $<TARGET_FILE:til_resource_test> )