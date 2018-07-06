find_package(PythonLibs)

pybind11_add_module(vampyr SHARED
    vampyr.cpp
    PyBoundingBox.cpp
    project.cpp
    operators.cpp
    methods.cpp
    bases.cpp
    fundamentalTypes.cpp
)

target_include_directories(vampyr PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/pybind11/include/
    )

target_link_libraries(vampyr PRIVATE
    mrcpp
    )
