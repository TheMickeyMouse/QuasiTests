set(PROJECT_NAME Testing)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "src/Advanced/TestAdvancedLighting.cpp"
    "src/Advanced/TestAdvancedLighting.h"
    "src/Advanced/TestCubeMap.cpp"
    "src/Advanced/TestCubeMap.h"
    "src/Advanced/TestFontRender.cpp"
    "src/Advanced/TestFontRender.h"
    "src/Advanced/TestLightCasters.cpp"
    "src/Advanced/TestLightCasters.h"
    "src/Advanced/TestMaterialMaps.cpp"
    "src/Advanced/TestMaterialMaps.h"
    "src/Advanced/TestPostProcessing.cpp"
    "src/Advanced/TestPostProcessing.h"
    "src/Advanced/TestStencilOutline.cpp"
    "src/Advanced/TestStencilOutline.h"
    "src/Basic/TestBatchedTextured.cpp"
    "src/Basic/TestBatchedTextured.h"
    "src/Basic/TestClearColor.cpp"
    "src/Basic/TestClearColor.h"
    "src/Basic/TestCubeRender.cpp"
    "src/Basic/TestCubeRender.h"
    "src/Basic/TestDynamicQuadGeometry.cpp"
    "src/Basic/TestDynamicQuadGeometry.h"
    "src/Basic/TestDynamicVertexGeometry.cpp"
    "src/Basic/TestDynamicVertexGeometry.h"
    "src/Demos/DemoFlappyBird.cpp"
    "src/Demos/DemoFlappyBird.h"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "src/Test.h"
    "src/TestingFramework.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "src/TestMenu.cpp"
    "Testing.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Testing)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/out/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME}/out/${PROJECT_NAME}/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/out/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME}/out/${PROJECT_NAME}/"
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/OpenGL/src;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Debug;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/GLs;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/Graphicals;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/Primitives;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/Meshes;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/Utils/Textures;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/Utils/Fonts;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Graphics/Utils/Model Loading;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/IO;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/Maths;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/vendor/imgui;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/vendor/freetype;"
    "${PROJECT_SOURCE_DIR}/OpenGL/src/stdu;"
    "${PROJECT_SOURCE_DIR}/Dependencies/GLEW/include;"
    "${PROJECT_SOURCE_DIR}/Dependencies/GLFW/include"
)

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE;"
        "GLEW_STATIC"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE;"
        "GLEW_STATIC"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )
endif()

# add dlls from opengl
add_custom_command(
    TARGET Testing PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}\\bin\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>\\OpenGL.dll"
    "${PROJECT_SOURCE_DIR}\\out\\$<CONFIG>\\${CMAKE_VS_PLATFORM_NAME}\\out\\Testing\\")

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "OpenGL"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
)

