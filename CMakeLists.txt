cmake_minimum_required(VERSION 2.6)
project(Rabbits_Qemu_Components)

find_package(Rabbits REQUIRED)

include_directories(
	"${SC_QEMU_DIR}/rabbits"
	"${PROJECT_SOURCE_DIR}"
)

add_subdirectory(libsc_qemu)
add_subdirectory(qemu)
add_subdirectory(components)

rabbits_add_dynlib(qemu)
