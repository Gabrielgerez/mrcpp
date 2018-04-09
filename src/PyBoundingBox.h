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

template<int D>
class PyBoundingBox : public mrcpp::BoundingBox<D> {
public:
    using mrcpp::BoundingBox<D>::BoundingBox;
    PyBoundingBox(int, pybind11::array_t<int>, pybind11::array_t <int>);
};

