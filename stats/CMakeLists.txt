cmake_minimum_required(VERSION 3.27.0 FATAL_ERROR)
include(CMakePrintHelpers)
project(imgui)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(GL_SILENCE_DEPRECATION)

# for glfw3 and vulkan
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

# will go in bumdle Resource folder
set(sharedRes
    SF-Pro-Text-Regular.otf
)
set_source_files_properties(${sharedRes} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

find_package(glfw3 REQUIRED)


add_executable(AntChart MACOSX_BUNDLE AntChart.cpp ../Graph.cpp ../GraphSalesman.cpp implot.h implot_internal.h implot.cpp implot_items.cpp imgui.cpp imgui_tables.cpp imgui_widgets.cpp imgui_draw.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp ${sharedRes})
target_link_libraries(AntChart glfw3 sfml-window sfml-system sfml-graphics "-framework OpenGL -framework IOKit -framework Cocoa")


install(CODE [[
    SET(BU_COPY_FULL_FRAMEWORK_CONTENTS ON)
    include(BundleUtilities)

    fixup_bundle("${CMAKE_INSTALL_PREFIX}/stats/AntChart.app" "" "")
    execute_process (COMMAND bash -c "codesign --force --deep --sign - ./stats/AntChart.app")
    execute_process (COMMAND bash -c "codesign -dv --verbose=4 ./stats/AntChart.app")

]] COMPONENT Runtime)


# add_custom_target(format
#   COMMAND bash -c "clang-format --verbose -i *.cpp *.h"
#   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )
