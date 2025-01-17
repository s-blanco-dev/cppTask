include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Incluir los directorios del proyecto principal
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Recopilar todos los archivos fuente del proyecto principal, excluyendo main.cpp
file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*main\\.cpp$") # Excluye main.cpp

# Agregar el ejecutable de pruebas
add_executable(FacadeTests FacadeTests.cpp ${PROJECT_SOURCES})

# Enlazar las dependencias
target_link_libraries(FacadeTests
  PRIVATE Catch2::Catch2WithMain
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE nlohmann_json
)

# Descubrir los tests
catch_discover_tests(FacadeTests)

