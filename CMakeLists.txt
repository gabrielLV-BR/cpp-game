cmake_minimum_required(VERSION 3.17)
project(MyGame VERSION 0.0)

add_subdirectory("${CMAKE_SOURCE_DIR}/modules/raylib")
add_subdirectory("${CMAKE_SOURCE_DIR}/modules/entt")

add_executable(
	${PROJECT_NAME}
	src/main.cpp

	src/entities/player.cpp
	src/entities/player.hpp

	src/components/components.hpp

	src/core/graphics/window.hpp
	src/core/graphics/window.cpp
	src/core/graphics/renderer.hpp
	src/core/graphics/renderer.cpp

	src/core/ecs/world.cpp
	src/core/ecs/world.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/modules/raylib/src/")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/modules/entt/src/")

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
