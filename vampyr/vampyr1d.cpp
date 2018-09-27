/*
 *  \date Mar 08, 2018
 *  \author Magnar Bjørgve <magnar.bjorgve@uit.no> \n
 *          Hylleraas Centre for Quantum Molecular Sciences \n
 *          UiT - The Arctic University of Norway
 */


#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "pybind11/eigen.h"
#include "pybind11/functional.h"

#include "trees/MultiResolutionAnalysis.h"
#include "trees/MWTree.h"
#include "trees/FunctionTree.h"
#include "trees/FunctionTreeVector.h"

#include "PyBoundingBox.h"
#include "PyAnalyticFunction.h"
#include "project.h"

#include "operators/DerivativeOperator.h"
#include "operators/ABGVOperator.h"
#include "operators/ConvolutionOperator.h"

#include "treebuilders/add.h"
#include "treebuilders/multiply.h"
#include "treebuilders/apply.h"
#include "treebuilders/project.h"



using namespace mrcpp;
namespace py = pybind11;

void bases(py::module &);

PYBIND11_MODULE(vampyr1d, m) {

m.def("project", py::overload_cast<double, FunctionTree<1> &, std::function<double (double)>, int>(&project1D),
    py::arg("prec"), py::arg("out"), py::arg("func"), py::arg("maxIter")= -1);

py::class_<PyBoundingBox<1>> (m, "BoundingBox1D")
    .def(py::init<int, py::array_t<int>, py::array_t <int>>())
    .def(py::init<int, int *,  int *>()) //1D cases can be initialized without array type input
    .def("getScale", &PyBoundingBox<1>::getScale);

py::class_<MultiResolutionAnalysis<1>> (m, "MultiResolutionAnalysis1D")
    .def(py::init<PyBoundingBox<1>, ScalingBasis, int>())
    .def("getOrder", &MultiResolutionAnalysis<1>::getOrder)
    .def("getMaxDepth", &MultiResolutionAnalysis<1>::getMaxDepth)
    .def("getMaxScale", &MultiResolutionAnalysis<1>::getMaxScale);

py::class_<MWTree<1>> mwtree(m, "MWTree");
mwtree
    .def(py::init<MultiResolutionAnalysis<1>>())
    .def("getSquareNorm", &MWTree<1>::getSquareNorm);

py::class_<FunctionTree<1>> functree(m, "FunctionTree1D", mwtree);
functree
    .def(py::init<MultiResolutionAnalysis<1>>())
    .def("integrate", &FunctionTree<1>::integrate)
    .def("clear", &FunctionTree<1>::clear)
    .def("normalize", &FunctionTree<1>::normalize)
    .def("evalf", py::overload_cast<double>(&FunctionTree<1>::evalf));

m.def("apply", py::overload_cast<double, FunctionTree<1> &, ConvolutionOperator<1> &, FunctionTree<1> &, int>(&apply<1>),
    py::arg("prec"), py::arg("out"), py::arg("oper"), py::arg("inp"), py::arg("maxIter") = -1);
m.def("apply", py::overload_cast<FunctionTree<1> &, DerivativeOperator<1> &, FunctionTree<1> &, int>(&apply<1>));

bases(m); // Import bases

m.def("add", py::overload_cast<double, FunctionTree<1> &, double, FunctionTree<1> &, double, FunctionTree<1> &, int>(&add<1>),
    py::arg("prec"), py::arg("out"), py::arg("a"),  py::arg("tree_a"), py::arg("b"), py::arg("tree_b"), py::arg("maxIter") = -1,
    "Adds to function trees");

m.def("project", py::overload_cast<double, FunctionTree<1> &, RepresentableFunction<1> &, int>(&project<1>),
    py::arg("prec"), py::arg("out"), py::arg("inp"), py::arg("maxIter")= -1);

m.def("multiply", py::overload_cast<double, FunctionTree<1> &, double, FunctionTree<1> &, FunctionTree<1> &, int >(&multiply<1>),
    py::arg("prec"), py::arg("out"), py::arg("c"), py::arg("tree_a"), py::arg("tree_b"), py::arg("maxIter") = -1,
    "Multiplies two function trees");

py::class_<DerivativeOperator<1>> deriv(m, "Derivative Operator");
deriv
    .def(py::init<MultiResolutionAnalysis<1>>());

py::class_<ABGVOperator<1>> (m, "ABGVOperator", deriv)
    .def(py::init< MultiResolutionAnalysis<1> &, double, double >());
}
