#.rst:
# Findsc-qemu
# -----------
#
# Try to find sc-qemu

set(_SC_QEMU_PATHS PATHS
	${SC_QEMU_PREFIX}
	$ENV{SC_QEMU_PREFIX}
)

find_path(SC_QEMU_INCLUDE_DIR sc-qemu.h ${_SC_QEMU_PATHS} PATH_SUFFIXES include/sc-qemu)

if (NOT SC_QEMU_LIBRARIES)
	find_library(SC_QEMU_LIBRARY_RELEASE NAMES qemu-system-arm ${_SC_QEMU_PATHS} PATH_SUFFIXES lib/rabbits)
	include(SelectLibraryConfigurations)
	SELECT_LIBRARY_CONFIGURATIONS(SC_QEMU)
endif ()

if (SC_QEMU_INCLUDE_DIR AND EXISTS "${SC_QEMU_INCLUDE_DIR}/sc-qemu/sc-qemu.h")
	set(SC_QEMU_VERSION "1.0.0")
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SC_QEMU
	                          REQUIRED_VARS SC_QEMU_LIBRARIES SC_QEMU_INCLUDE_DIR
				  VERSION_VAR SC_QEMU_VERSION)

