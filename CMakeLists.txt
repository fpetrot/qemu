cmake_minimum_required(VERSION 2.6)
project(Rabbits_Qemu_Components)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Rabbits REQUIRED)
find_package(sc-qemu REQUIRED)

include_directories(
	"${SC_QEMU_INCLUDE_DIR}"
	"${PROJECT_SOURCE_DIR}"
)

add_subdirectory(libsc_qemu)
add_subdirectory(qemu)
add_subdirectory(components)

rabbits_add_dynlib(qemu)
