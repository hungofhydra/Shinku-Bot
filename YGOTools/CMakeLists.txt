cmake_minimum_required(VERSION 3.14)
project(YGOTools VERSION 0.0.1 DESCRIPTION "A minimal YGOTools of using pybind11")


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto=auto -Wno-odr -Wno-lto-type-mismatch -Wno-unused-result")

find_package(Python 3.10 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG)
if(NOT pybind11_FOUND)
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -c
      "import os; import pybind11; print(os.path.dirname(pybind11.get_cmake_dir()))"
    OUTPUT_VARIABLE _tmp_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
  find_package(pybind11 CONFIG REQUIRED)
endif()

file(GLOB SOURCES src/*.cpp)
pybind11_add_module(YGOTools ${SOURCES})

target_include_directories(YGOTools PRIVATE include)
target_compile_features(YGOTools PRIVATE cxx_std_20)



find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(SQLite3 REQUIRED)
target_link_libraries(YGOTools PRIVATE ICU::uc ICU::i18n SQLite::SQLite3 pybind11::embed)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py.in ${Python_SITEARCH}/YGOTools/__init__.py @ONLY)


if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(YGOTools)
endif()


install(TARGETS YGOTools
  EXPORT YGOToolsTargets
  RUNTIME DESTINATION ${Python_SITEARCH}/YGOTools
  LIBRARY DESTINATION ${Python_SITEARCH}/YGOTools
)

add_custom_command(TARGET YGOTools POST_BUILD COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR})