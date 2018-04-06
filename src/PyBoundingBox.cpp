/*
 *  \date Apr 06, 2018
 *  \author Magnar Bjørgve <magnar.bjorgve@uit.no> \n
 *          Hylleraas Centre for Quantum Molecular Sciences \n
 *          UiT - The Arctic University of Norway
 */

#include "PyBoundingBox.h"
using namespace std;
using namespace mrcpp;

template<int D>
PyBoundingBox<D>::PyBoundingBox(int n, py::array_t<int> l, py::array_t<int> nb)
        : BoundingBox<D>() {

    auto bufl = l.request();
    auto bufnb = nb.request();

    if (bufl.ndim != 1 or bufnb.ndim != 1)
        std::runtime_error("Number of dimensions must be one");

    const int *lPtr = (const int *) bufl.ptr;
    const int *nbPtr = (const int *) bufnb.ptr;


    this->cornerIndex = NodeIndex<D>(n, lPtr);
    this->setNBoxes(nbPtr);
    this->setDerivedParameters();
}

template class PyBoundingBox<1>;
template class PyBoundingBox<2>;
template class PyBoundingBox<3>;
