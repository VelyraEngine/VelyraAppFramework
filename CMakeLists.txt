cmake_minimum_required(VERSION 3.22)
project(VelyraAppFramework DESCRIPTION "Small App Framework to quickly build ImGui apps")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

set(VELYRA_UTILS_PATH)
vl_include_or_fetch(VelyraUtils VELYRA_UTILS_PATH)
message(STATUS "VelyraUtils found at ${VELYRA_UTILS_PATH}")

set(VELYRA_CORE_PATH)
vl_include_or_fetch(VelyraCore VELYRA_CORE_PATH)
message(STATUS "VelyraCore found at ${VELYRA_CORE_PATH}")

set(VELYRA_APP_FRAMEWORK_HEADERS
    include/VelyraAppFramework/AppInclude.hpp
    include/VelyraAppFramework/Layer.hpp
    include/VelyraAppFramework/Application.hpp
    include/VelyraAppFramework/AppData.hpp
    include/VelyraAppFramework/Settings.hpp
    include/VelyraAppFramework/Styles.hpp
    include/VelyraAppFramework/Layout.hpp

    include/VelyraAppFramework/LayoutEngine/LayoutEngine.hpp
    include/VelyraAppFramework/LayoutEngine/LayoutDefinitions.hpp
    include/VelyraAppFramework/LayoutEngine/LayoutDescription.hpp
    src/VelyraAppFramework/LayoutEngine/LayoutNode.hpp

    include/VelyraAppFramework/Widgets/Popup.hpp
    include/VelyraAppFramework/Widgets/RegexInput.hpp
    include/VelyraAppFramework/Widgets/Panel.hpp
    include/VelyraAppFramework/Widgets/IPanel.hpp
    include/VelyraAppFramework/Widgets/Terminal.hpp

    src/VelyraAppFramework/Shared/LoggerNames.hpp
    src/VelyraAppFramework/FrameworkSettings.hpp
    src/VelyraAppFramework/Settings/GitSync.hpp
    src/VelyraAppFramework/Settings/JsonMerge.hpp
)

set(VELYRA_APP_FRAMEWORK_SRC
    src/VelyraAppFramework/Layer.cpp
    src/VelyraAppFramework/Layout.cpp
    src/VelyraAppFramework/Application.cpp
    src/VelyraAppFramework/AppData.cpp
    src/VelyraAppFramework/Settings.cpp
    src/VelyraAppFramework/Settings/GitSync.cpp
    src/VelyraAppFramework/Settings/JsonMerge.cpp
    src/VelyraAppFramework/AppInclude.cpp

    src/VelyraAppFramework/LayoutEngine/LayoutEngine.cpp
    src/VelyraAppFramework/LayoutEngine/LayoutDescription.cpp

    src/VelyraAppFramework/Widgets/Popup.cpp
    src/VelyraAppFramework/Widgets/RegexInput.cpp
    src/VelyraAppFramework/Widgets/Panel.cpp
    src/VelyraAppFramework/Widgets/IPanel.cpp
    src/VelyraAppFramework/Widgets/Terminal.cpp
)

add_library(VelyraAppFramework STATIC ${VELYRA_APP_FRAMEWORK_HEADERS} ${VELYRA_APP_FRAMEWORK_SRC})
vl_configure_target(VelyraAppFramework)
target_precompile_headers(VelyraAppFramework PRIVATE src/VelyraAppFramework/Shared/Pch.hpp)
target_include_directories(VelyraAppFramework PUBLIC include)
target_include_directories(VelyraAppFramework PRIVATE src/VelyraAppFramework/Shared)
target_link_libraries(VelyraAppFramework PUBLIC VelyraCore)

################# Testing #################

set(VELYRA_APP_FRAMEWORK_TEST_SRC
    test/TestLayoutDescription.cpp
    test/TestLayoutEngine.cpp
    test/TestSettings.cpp
    test/TestJsonMerge.cpp
)


if (BUILD_TESTING)
    add_executable(TestVelyraAppFramework ${VELYRA_APP_FRAMEWORK_TEST_SRC})
    target_include_directories(TestVelyraAppFramework PUBLIC include)
    target_include_directories(TestVelyraAppFramework PRIVATE include/VelyraCore/ImGui)
    target_precompile_headers(TestVelyraAppFramework PUBLIC test/TestPch.hpp)
    target_link_libraries(TestVelyraAppFramework PUBLIC VelyraAppFramework)
    vl_configure_test_target(TestVelyraAppFramework)
endif ()

################# Development #################

add_executable(VelyraAppFrameworkMain main.cpp)
target_link_libraries(VelyraAppFrameworkMain PRIVATE VelyraAppFramework)
vl_configure_target(VelyraAppFrameworkMain)