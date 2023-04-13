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

# Test GJKPolygon Code
add_executable(gjk_shape_polyhedron_test Utilities/Collision/GJKPolyhedron.cpp)
target_link_libraries(gjk_shape_polyhedron_test PRIVATE FC_IFF_IO)
add_test( NAME gjk_shape_polyhedron_test COMMAND $<TARGET_FILE:gjk_shape_polyhedron_test> )

# Test GJK Code
add_executable(gjk_test Utilities/Collision/GJK.cpp)
target_link_libraries(gjk_test PRIVATE FC_IFF_IO)
add_test( NAME gjk_test COMMAND $<TARGET_FILE:gjk_test> )

# Test Grid2D Code
add_executable(grid_2d_test Utilities/Grid2D.cpp)
target_link_libraries(grid_2d_test PRIVATE FC_IFF_IO)
add_test( NAME grid_2d_test COMMAND $<TARGET_FILE:grid_2d_test> )

# Test GridSq2D Code
add_executable(grid_sq_2d_test Utilities/GridSq2D.cpp)
target_link_libraries(grid_sq_2d_test PRIVATE FC_IFF_IO)
add_test( NAME grid_sq_2d_test COMMAND $<TARGET_FILE:grid_sq_2d_test> )

# Test PixelFormat Code
add_executable(pixel_format_test Utilities/PixelFormat.cpp)
target_link_libraries(pixel_format_test PRIVATE FC_IFF_IO)
add_test( NAME pixel_format_test COMMAND $<TARGET_FILE:pixel_format_test> )

# Test Random Code
add_executable(random_test Utilities/Random.cpp)
target_link_libraries(random_test PRIVATE FC_IFF_IO)
add_test( NAME random_test COMMAND $<TARGET_FILE:random_test> )

# Test Image2D Code
add_executable(image_2d_test Utilities/Image2D.cpp)
target_link_libraries(image_2d_test PRIVATE FC_IFF_IO)
add_test( NAME image_2d_test COMMAND $<TARGET_FILE:image_2d_test> )

# Test ImagePalete2D Code
add_executable(image_palette_2D_test Utilities/ImagePalette2D.cpp)
target_link_libraries(image_palette_2D_test PRIVATE FC_IFF_IO)
add_test( NAME image_palette_2D_test COMMAND $<TARGET_FILE:image_palette_2D_test> )

# Test Quite Okay Image Code
add_executable(qoi_image_format_test Utilities/ImageFormat/QuiteOkImage.cpp)
target_link_libraries(qoi_image_format_test PRIVATE FC_IFF_IO)
add_test( NAME qoi_image_format_test COMMAND $<TARGET_FILE:qoi_image_format_test> )

# Test Portable Network Graphics Code
add_executable(png_image_format_test Utilities/ImageFormat/PortableNetworkGraphics.cpp)
target_link_libraries(png_image_format_test PRIVATE FC_IFF_IO)
add_test( NAME png_image_format_test COMMAND $<TARGET_FILE:png_image_format_test> )

# Test Model Builder Code
add_executable(model_builder_test Utilities/ModelBuilder.cpp)
target_link_libraries(model_builder_test PRIVATE FC_IFF_IO)
add_test( NAME model_builder_test COMMAND $<TARGET_FILE:model_builder_test> )

# Test IFFOptions Code
add_executable(iff_options_test Data/Mission/IFFOptions.cpp)
target_link_libraries(iff_options_test PRIVATE FC_IFF_IO)
add_test( NAME iff_options_test COMMAND $<TARGET_FILE:iff_options_test> )

# Test ANMResource Code
add_executable(anm_resource_test Data/Mission/ANMResource.cpp)
target_link_libraries(anm_resource_test PRIVATE FC_IFF_IO)
add_test( NAME anm_resource_test COMMAND $<TARGET_FILE:anm_resource_test> )

# Test FontResource Code
add_executable(fnt_resource_test Data/Mission/FontResource.cpp)
target_link_libraries(fnt_resource_test PRIVATE FC_IFF_IO)
add_test( NAME fnt_resource_test COMMAND $<TARGET_FILE:fnt_resource_test> )

# Test TilResource Code
add_executable(til_resource_test Data/Mission/TilResource.cpp)
target_link_libraries(til_resource_test PRIVATE FC_IFF_IO)
add_test( NAME til_resource_test COMMAND $<TARGET_FILE:til_resource_test> )
