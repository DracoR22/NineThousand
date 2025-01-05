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

# assimp
add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES 
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/third_party/dll/assimp-vc143-mtd.dll
    IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/lib/assimp-vc143-mtd.lib
)
target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include)

# physx
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(GLOB_RECURSE PHYSX_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/physx/lib/debug/*.lib")
else()
    file(GLOB_RECURSE PHYSX_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/physx/lib/release/*.lib")
endif()

file(GLOB_RECURSE PHYSX_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/dll/*.dll")

# Link all files inside /src
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Create the executable first
add_executable(NineThousand ${SOURCES})

# Set include directories and libraries after the target is created
target_include_directories(NineThousand PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/physx/include)
target_link_libraries(NineThousand PRIVATE glfw glad glm stb_image assimp ${PHYSX_LIBS})

# Include resources folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources)

# Add PhysX DLLs to output folder
foreach(DLL ${PHYSX_DLLS})
    add_custom_command(TARGET NineThousand POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL} $<TARGET_FILE_DIR:NineThousand>
    )
endforeach()
