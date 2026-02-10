cmake_minimum_required(VERSION 3.22)
project(VelyraAppFramework DESCRIPTION "Small App Framework to quickly build ImGui apps")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

set(VELYRA_UTILS_PATH)
include_or_fetch(VelyraUtils VELYRA_UTILS_PATH)
message(STATUS "VelyraUtils found at ${VELYRA_UTILS_PATH}")

set(VELYRA_CORE_PATH)
include_or_fetch(VelyraCore VELYRA_CORE_PATH)
message(STATUS "VelyraCore found at ${VELYRA_CORE_PATH}")

set(VELYRA_APP_FRAMEWORK_HEADERS
    include/VelyraAppFramework/Application.hpp
    include/VelyraAppFramework/AppData.hpp
    include/VelyraAppFramework/AppInclude.hpp
    include/VelyraAppFramework/Layer.hpp
    include/VelyraAppFramework/Settings.hpp

    include/VelyraAppFramework/LayoutEngine/LayoutEngine.hpp
    include/VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp
    include/VelyraAppFramework/LayoutEngine/LayoutDescription.hpp
    include/VelyraAppFramework/LayoutEngine/LayoutNode.hpp

    src/VelyraAppFramework/LoggerNames.hpp
)

set(VELYRA_APP_FRAMEWORK_SRC
    src/VelyraAppFramework/Application.cpp
    src/VelyraAppFramework/AppData.cpp
    src/VelyraAppFramework/Settings.cpp

    src/VelyraAppFramework/LayoutEngine/LayoutEngine.cpp
    src/VelyraAppFramework/LayoutEngine/LayoutNode.cpp
    src/VelyraAppFramework/LayoutEngine/LayoutDescription.cpp
)

add_library(VelyraAppFramework STATIC ${VELYRA_APP_FRAMEWORK_HEADERS} ${VELYRA_APP_FRAMEWORK_SRC})
velyra_target_set_compile_flags(VelyraAppFramework)
target_precompile_headers(VelyraAppFramework PRIVATE src/Pch.hpp)
target_include_directories(VelyraAppFramework PUBLIC include)
target_include_directories(VelyraAppFramework PRIVATE src) # For the PCH
target_link_libraries(VelyraAppFramework PUBLIC VelyraCore)
target_compile_definitions(VelyraAppFramework PRIVATE
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
    $<$<NOT:$<CONFIG:Debug>>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>)

################# Testing #################

set(VELYRA_APP_FRAMEWORK_TEST_SRC
    test/TestLayoutDescription.cpp
    test/TestLayoutEngine.cpp
)


if (BUILD_TESTING)
    fetch_gtest()

    add_executable(TestVelyraAppFramework ${VELYRA_APP_FRAMEWORK_TEST_SRC})
    target_include_directories(TestVelyraAppFramework PUBLIC include)
    target_include_directories(TestVelyraAppFramework PRIVATE include/VelyraCore/ImGui)
    target_precompile_headers(TestVelyraAppFramework PUBLIC test/TestPch.hpp)
    target_link_libraries(TestVelyraAppFramework PUBLIC GTest::gtest GTest::gtest_main VelyraAppFramework)
    add_test(NAME CTestVelyraAppFramework COMMAND ${PROOT} $<TARGET_FILE:TestVelyraAppFramework>)
    set_tests_properties(CTestVelyraAppFramework PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif ()

################# Development #################

add_executable(VelyraAppFrameworkMain main.cpp)
target_link_libraries(VelyraAppFrameworkMain PRIVATE VelyraAppFramework)