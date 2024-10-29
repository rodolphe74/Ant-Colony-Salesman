cmake_minimum_required(VERSION 3.0)
project(Salesman)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

# need apple CoreFoundation to find Resources path in Macos X app bundle
find_library(CoreFoundation_Library CoreFoundation)

# will go in bumdle Resource folder
set(sharedRes
    Arial.ttf
)
set_source_files_properties(${sharedRes} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

# note the $sharedRes folder dependency
add_executable(SfmlSalesman MACOSX_BUNDLE SfmlMain.cpp Graph.cpp GraphSalesman.cpp ${sharedRes})
target_link_libraries(SfmlSalesman sfml-window sfml-system sfml-graphics ${CoreFoundation_Library})

add_executable(Salesman Main.cpp Graph.cpp GraphSalesman.cpp)
target_link_libraries(Salesman sfml-window sfml-system sfml-graphics ${CoreFoundation_Library})


# make install will create and sign the app bundle
install(CODE [[
    SET(BU_COPY_FULL_FRAMEWORK_CONTENTS ON)
  include(BundleUtilities)
  fixup_bundle("${CMAKE_INSTALL_PREFIX}/SfmlSalesman.app" "" "")
  execute_process (COMMAND bash -c "codesign --force --deep --sign - ./SfmlSalesman.app")
  execute_process (COMMAND bash -c "codesign -dv --verbose=4 ./SfmlSalesman.app")
]] COMPONENT Runtime)


# other methods to copy Resources
# install(DIRECTORY Resources/ DESTINATION ${CMAKE_BINARY_DIR}/SfmlSalesman.app/Contents/Resources)
# file(COPY "../Arial.ttf"
#       DESTINATION "${CMAKE_BINARY_DIR}/SfmlSalesman.app/Contents/Resources")


add_custom_target(format
  COMMAND bash -c "clang-format --verbose -i *.cpp *.hpp"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
