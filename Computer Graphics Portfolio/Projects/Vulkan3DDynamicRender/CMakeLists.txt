

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

include(FetchContent)

project(KyleVulaknRenderer LANGUAGES C CXX)

if (NOT IS_DIRECTORY "$ENV{VULKAN_SDK}")
    message(WARNING "Vulkan SDK not detected. Set VULKAN_SDK env var (e.g. C:\\VulkanSDK\\1.x.x.x)")
else()
    message(STATUS "Found Vulkan SDK in $ENV{VULKAN_SDK}")
endif()

# Vulkan SDK headers (volk, vma, slang headers, etc.)
include_directories("$ENV{VULKAN_SDK}/include")


FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(GLM
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(GLM)

find_library(Slang_LIBRARY
    NAMES slang
    HINTS "$ENV{VULKAN_SDK}/lib"
    REQUIRED
)

add_executable(KyleVulaknRenderer)

target_sources(KyleVulaknRenderer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.slang"
)

target_compile_features(KyleVulaknRenderer PRIVATE cxx_std_20)

target_compile_definitions(KyleVulaknRenderer PRIVATE
    VK_NO_PROTOTYPES
    NOMINMAX
    _USE_MATH_DEFINES
)

if (WIN32)
    target_compile_definitions(KyleVulaknRenderer PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
        _CRT_SECURE_NO_WARNINGS
    )
endif()

set_target_properties(KyleVulaknRenderer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(KyleVulaknRenderer PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    glm::glm-header-only
    ${Slang_LIBRARY}
)

if (MSVC)
    target_link_options(KyleVulaknRenderer PRIVATE
        /SUBSYSTEM:CONSOLE
        /ENTRY:mainCRTStartup
    )
endif()
