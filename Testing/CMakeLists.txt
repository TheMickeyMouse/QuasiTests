set(PROJECT_NAME Testing)

################################################################################
# Source groups
################################################################################
set(HEADER_FILES
    src/Advanced/TestAdvancedLighting.h
    src/Advanced/TestCubeMap.h
    src/Advanced/TestFontRender.h
    src/Advanced/TestLightCasters.h
    src/Advanced/TestMaterialMaps.h
    src/Advanced/TestPostProcessing.h
    src/Advanced/TestStencilOutline.h
    src/Basic/TestBatchedTextured.h
    src/Basic/TestClearColor.h
    src/Basic/TestCubeRender.h
    src/Basic/TestDynamicQuadGeometry.h
    src/Basic/TestDynamicVertexGeometry.h
    src/Demos/DemoFlappyBird.h
    src/Advanced/TestGeometryShader.h
    src/Advanced/TestDrawInstances.h
    src/Advanced/TestShadowMap.h
    src/Physics/TestCircleCollision2D.h
    src/Test.h
    src/TestingFramework.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
    src/Advanced/TestAdvancedLighting.cpp
    src/Advanced/TestCubeMap.cpp
    src/Advanced/TestFontRender.cpp
    src/Advanced/TestLightCasters.cpp
    src/Advanced/TestMaterialMaps.cpp
    src/Advanced/TestPostProcessing.cpp
    src/Advanced/TestStencilOutline.cpp
    src/Basic/TestBatchedTextured.cpp
    src/Basic/TestClearColor.cpp
    src/Basic/TestCubeRender.cpp
    src/Basic/TestDynamicQuadGeometry.cpp
    src/Basic/TestDynamicVertexGeometry.cpp
    src/Demos/DemoFlappyBird.cpp
    src/Advanced/TestGeometryShader.cpp
    src/Advanced/TestDrawInstances.cpp
    src/Advanced/TestShadowMap.cpp
    src/Physics/TestCircleCollision2D.cpp
    src/TestMenu.cpp
    Testing.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

set(ALL_FILES
    ${HEADER_FILES}
    ${SOURCE_FILES}
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
target_include_directories(${PROJECT_NAME} PRIVATE src)

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

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "OpenGL"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES};")

#target_link_directories(${PROJECT_NAME} PRIVATE
#    "${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
#)

