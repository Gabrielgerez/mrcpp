find_package(PythonLibs)

pybind11_add_module(vampyr1d SHARED
    vampyr1d.cpp
    PyBoundingBox.cpp
    bases.cpp
    project.cpp
)

pybind11_add_module(vampyr2d SHARED
    vampyr2d.cpp
    PyBoundingBox.cpp
    bases.cpp
    project.cpp
)

pybind11_add_module(vampyr3d SHARED
    vampyr3d.cpp
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

target_include_directories(vampyr2d PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/pybind11/include/
)

target_include_directories(vampyr3d PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/pybind11/include/
)

target_link_libraries(vampyr1d PRIVATE
    mrcpp
)

target_link_libraries(vampyr2d PRIVATE
    mrcpp
)

target_link_libraries(vampyr3d PRIVATE
    mrcpp
)
