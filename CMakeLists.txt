cmake_minimum_required(VERSION 3.17)
project(MyGame VERSION 0.0)

add_subdirectory("${CMAKE_SOURCE_DIR}/modules/entt")
add_subdirectory("${CMAKE_SOURCE_DIR}/modules/glfw")

find_package(Vulkan REQUIRED)

add_executable(
	${PROJECT_NAME}
	src/main.cpp

	src/entities/player.cpp
	src/entities/player.hpp

	src/components/components.hpp

	src/core/vulkan/vkapp.hpp
	src/core/vulkan/vkapp.cpp

	src/core/vulkan/vkpipeline.hpp
	src/core/vulkan/vkpipeline.cpp

	src/core/vulkan/vkshader.hpp
	src/core/vulkan/vkshader.cpp
	
	src/core/vulkan/vkutils.hpp
	src/core/vulkan/vkutils.cpp
	
	src/core/vulkan/vkswapchain.hpp
	src/core/vulkan/vkswapchain.cpp
	
	src/core/vulkan/vkstructs.hpp

	src/core/graphics/renderer.hpp
	src/core/graphics/renderer.cpp

	src/core/ecs/world.hpp
	src/core/ecs/world.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS="${CMAKE_SOURCE_DIR}/assets/")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/modules/entt/src/")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)
