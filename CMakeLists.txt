cmake_minimum_required(VERSION "3.19.2")

project(NineThousand)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable visual studio hot-reload
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1")

# glfw
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/lib/glfw3.lib)
target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include)

# glad
add_library(glad ${PROJECT_SOURCE_DIR}/vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)

# stb_image
add_library(stb_image STATIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_image.cpp)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image)

# nlohmann JSON
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann_json/include)

# imGui
file(GLOB IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
)

# assimp
add_library(assimp SHARED IMPORTED)
set_target_properties(assimp PROPERTIES 
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/dll/assimp-vc143-mt.dll
    IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/lib/assimp-vc143-mt.lib
)
target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/include)

# physx
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(GLOB_RECURSE PHYSX_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/physx/lib/debug/*.lib")
else()
    file(GLOB_RECURSE PHYSX_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/physx/lib/release/*.lib")
endif()

# fmod
add_library(fmod STATIC IMPORTED)
set_target_properties(fmod PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmod/lib/fmod_vc.lib
)
target_include_directories(fmod INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/fmod/include)


file(GLOB_RECURSE ALL_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/dll/*.dll")

# Link all files inside /src
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

# Create the executable
add_executable(NineThousand ${SOURCES})

# Link all libraries
target_include_directories(NineThousand PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/physx/include)
target_link_libraries(NineThousand PRIVATE imgui glfw glad glm stb_image assimp fmod nlohmann_json ${PHYSX_LIBS})

# Include resources to output folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources)

# Add All DLLs to output folder
foreach(DLL ${ALL_DLLS})
    add_custom_command(TARGET NineThousand POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL} $<TARGET_FILE_DIR:NineThousand>
    )
endforeach()
