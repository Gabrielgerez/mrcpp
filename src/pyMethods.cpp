/*
 *  \date Mar 20, 2018
 *  \author Magnar Bjørgve <magnar.bjorgve@uit.no> \n
 *          Hylleraas Centre for Quantum Molecular Sciences \n
 *          UiT - The Arctic University of Norway
 */

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

#include "FunctionTree.h"
#include "FunctionTreeVector.h"
#include "add.h"
#include "multiply.h"
#include "apply.h"
#include "grid.h"
#include "project.h"
#include "DerivativeOperator.h"
#include "ConvolutionOperator.h"

namespace py = pybind11;
using namespace mrcpp;

void pyProject1D(py::module &m) {
    m.def("project", py::overload_cast<double, FunctionTree<1> &, std::function<double (double)>, int>(&project1D));
}

void pyProject2D(py::module &m) {
    m.def("project", py::overload_cast<double, FunctionTree<2> &, std::function<double (double, double)>, int>(&project2D));
}

void pyProject3D(py::module &m) {
    m.def("project", py::overload_cast<double, FunctionTree<3> &, std::function<double (double, double, double)>, int>(&project3D));
}

template <int D>
void pyMethods(py::module &m) {


    m.def("add", py::overload_cast<double, FunctionTree<D> &, double , FunctionTree<D> &, double, FunctionTree<D> &, int>(&add<D>));

    m.def("project", py::overload_cast<double, FunctionTree<D> &, RepresentableFunction<D> &, int>(&project<D>));

    m.def("multiply", py::overload_cast<double, FunctionTree<D> &, double, FunctionTree<D> &, FunctionTree<D> &, int >(&multiply<D>));
    m.def("multiply", py::overload_cast<double, FunctionTree<D> &, FunctionTreeVector<D> &, int >(&multiply<D>));

    m.def("build_grid", &build_grid<D>);
    m.def("copy_grid", py::overload_cast<FunctionTree<D> &, FunctionTree<D> &, int >(&copy_grid<D>));
    m.def("copy_grid", py::overload_cast<FunctionTree<D> &, FunctionTreeVector<D> &, int>(&copy_grid<D>));
    m.def("clear_grid", &clear_grid<D>);

    m.def("apply", py::overload_cast<double, FunctionTree<D> &, ConvolutionOperator<D> &, FunctionTree<D> &, int>(&apply<D>));
    m.def("apply", py::overload_cast<FunctionTree<D> &, DerivativeOperator<D> &, FunctionTree<D> &, int>(&apply<D>));
    m.def("dot", &dot<D>);
}

template void pyMethods<1>(py::module &m);
template void pyMethods<2>(py::module &m);
template void pyMethods<3>(py::module &m);
