cmake_minimum_required(VERSION 3.14)
project(activate-linux C)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(X11 REQUIRED)
find_package(Cairo REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(tgt_name activate-linux)
elseif(APPLE)
  set(tgt_name activate-macos)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  set(tgt_name activate-bsd)
else()
  message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()
add_executable(${tgt_name}
  src/activate_linux.c
  src/color.c
  )
target_link_libraries(${tgt_name} PRIVATE
  X11::X11
  X11::Xfixes
  X11::Xinerama
  Cairo::Cairo
  )

install(TARGETS ${tgt_name})
