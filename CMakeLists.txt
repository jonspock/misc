CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("devault tests")

add_subdirectory(pyplot)

FIND_PACKAGE(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS}  ./pyplot)

SET(TARGETS
  subsidy
	)

FOREACH(mod ${TARGETS})
	SET(SRC test_${mod}.cpp)
	ADD_EXECUTABLE(test_${mod} ${SRC})
  TARGET_LINK_LIBRARIES(test_${mod} plotlib ${PYTHON_LIBRARY})
	set_property(TARGET test_${mod} PROPERTY CXX_STANDARD 11)
	set_property(TARGET test_${mod} PROPERTY POSITION_INDEPENDENT_CODE TRUE)
ENDFOREACH(mod ${EXTS})
