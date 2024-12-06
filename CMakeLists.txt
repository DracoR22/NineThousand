cmake_minimum_required(VERSION "3.19.2")

project(NineThousand)

# glfw
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/lib/glfw3.lib)
target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include)

# glad
add_library(glad ${PROJECT_SOURCE_DIR}/third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)

# stb_image
add_library(stb_image STATIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/stb_image.cpp)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image)

# Gather all source files in src/ recursively
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Include the resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources)

add_executable(NineThousand ${SOURCES})
target_link_libraries(NineThousand PRIVATE glfw glad glm stb_image)
