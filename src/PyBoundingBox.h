/*
 *  \date Apr 06, 2018
 *  \author Magnar Bjørgve <magnar.bjorgve@uit.no> \n
 *          Hylleraas Centre for Quantum Molecular Sciences \n
 *          UiT - The Arctic University of Norway
 */

#pragma once

#include "BoundingBox.h"
#include "pybind11/include/pybind11/pybind11.h"
#include "pybind11/include/pybind11/numpy.h"

using namespace mrcpp;
namespace py = pybind11;
template<int D>
class PyBoundingBox : public BoundingBox<D> {
public:
    using BoundingBox<D>::BoundingBox;
    PyBoundingBox(int, py::array_t<int>, py::array_t <int>);
//    virtual ~PyBoundingBox() { }
};

