find_package(PythonLibs)

pybind11_add_module(vampyr1d SHARED
    vampyr1d.cpp
    PyBoundingBox.cpp
    bases.cpp
    project.cpp
)

target_include_directories(vampyr1d PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/pybind11/include/
    )

target_link_libraries(vampyr1d PRIVATE
    mrcpp
    )
