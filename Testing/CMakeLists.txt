set(PROJECT_NAME Testing)

set(HEADER_FILES
    src/Advanced/TestAdvancedLighting.h
    src/Advanced/TestCubeMap.h
    src/Advanced/TestFontRender.h
    src/Advanced/TestLightCasters.h
    src/Advanced/TestMaterialMaps.h
    src/Advanced/TestPostProcessing.h
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
    src/Physics/TestPhysicsPlayground2D.h
    src/Test.h
    src/TestManager.h
    src/TestMenu.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
    src/Advanced/TestAdvancedLighting.cpp
    src/Advanced/TestCubeMap.cpp
    src/Advanced/TestFontRender.cpp
    src/Advanced/TestLightCasters.cpp
    src/Advanced/TestMaterialMaps.cpp
    src/Advanced/TestPostProcessing.cpp
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
    src/Physics/TestPhysicsPlayground2D.cpp
    src/TestManager.cpp
    src/TestMenu.cpp
    Testing.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

set(ALL_FILES
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

add_executable(${PROJECT_NAME} ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PUBLIC Quasi)
