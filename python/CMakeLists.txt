include(DefaultPathsMRCPP)

find_package(PythonLibs)

add_subdirectory(pybind11)

pybind11_add_module(pymrcpp SHARED
    pymrcpp.cpp
    pyBases.cpp
    #pyFundamentalTypes.cpp
    #pyGauss.cpp
    pyMethods.cpp
    pyOperators.cpp
)

target_include_directories(pymrcpp PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/python/pybind11/include/
    )
