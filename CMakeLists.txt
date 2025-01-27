cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 20)
project(cppTask)

find_package(Catch2 3 REQUIRED)
include(CTest)

find_package(TBB REQUIRED)

# Agregar la biblioteca Catch2 usando FetchContent
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0 # Usa la versión más reciente estable
)
FetchContent_MakeAvailable(Catch2)


list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
 
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)
 

add_executable(cppTask 
  src/main.cpp
  src/Task.cpp
  src/Priority.cpp
  include/Task.h
  include/TaskManager.h
  src/TaskManager.cpp
  src/Facade.cpp
  include/Facade.h
  src/tui/MainDialog.cpp
  include/Priority.h
  include/tui/MainDialog.h
  include/tui/UserPrompts.h
  src/tui/UserPrompts.cpp
)

add_subdirectory(external/json)

target_link_libraries(cppTask
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PRIVATE nlohmann_json
  PRIVATE TBB::tbb
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20 # or 20 if you want something more recent
        CXX_STANDARD_REQUIRED ON
)

# for Catch2 unit tests
enable_testing()
add_subdirectory(tests)
