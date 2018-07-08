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

#include "trees/BoundingBox.h"
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

PYBIND11_MODULE(vampyr2d, m) {

m.def("project", py::overload_cast<double, FunctionTree<2> &, std::function<double (double, double)>, int>(&project2D),
    py::arg("prec"), py::arg("out"), py::arg("func"), py::arg("maxIter")= -1);

py::class_<PyBoundingBox<2>> (m, "BoundingBox2D")
    .def(py::init<int, py::array_t<int>, py::array_t <int>>())
    .def(py::init<int, int *,  int *>()) //1D cases can be initialized without array type input
    .def("getScale", &PyBoundingBox<2>::getScale);

py::class_<MultiResolutionAnalysis<2>> (m, "MultiResolutionAnalysis2D")
    .def(py::init<PyBoundingBox<2>, ScalingBasis, int>())
    .def("getOrder", &MultiResolutionAnalysis<2>::getOrder)
    .def("getMaxDepth", &MultiResolutionAnalysis<2>::getMaxDepth)
    .def("getMaxScale", &MultiResolutionAnalysis<2>::getMaxScale);

py::class_<MWTree<2>> mwtree(m, "MWTree");
mwtree
    .def(py::init<MultiResolutionAnalysis<2>>())
    .def("getSquareNorm", &MWTree<2>::getSquareNorm);

py::class_<FunctionTree<2>> functree(m, "FunctionTree2D", mwtree);
functree
    .def(py::init<MultiResolutionAnalysis<2>>())
    .def("integrate", &FunctionTree<2>::integrate)
    .def("clear", &FunctionTree<2>::clear)
    .def("normalize", &FunctionTree<2>::normalize)
    .def("evalf", py::overload_cast<double, double>(&FunctionTree<2>::evalf));

m.def("apply", py::overload_cast<double, FunctionTree<2> &, ConvolutionOperator<2> &, FunctionTree<2> &, int>(&apply<2>),
    py::arg("prec"), py::arg("out"), py::arg("oper"), py::arg("inp"), py::arg("maxIter") = -1);
m.def("apply", py::overload_cast<FunctionTree<2> &, DerivativeOperator<2> &, FunctionTree<2> &, int>(&apply<2>));

bases(m);

m.def("add", py::overload_cast<double, FunctionTree<2> &, double, FunctionTree<2> &, double, FunctionTree<2> &, int>(&add<2>),
    py::arg("prec"), py::arg("out"), py::arg("a"),  py::arg("tree_a"), py::arg("b"), py::arg("tree_b"), py::arg("maxIter") = -1,
    "Adds to function trees");

m.def("project", py::overload_cast<double, FunctionTree<2> &, RepresentableFunction<2> &, int>(&project<2>),
    py::arg("prec"), py::arg("out"), py::arg("inp"), py::arg("maxIter")= -1);

m.def("multiply", py::overload_cast<double, FunctionTree<2> &, double, FunctionTree<2> &, FunctionTree<2> &, int >(&multiply<2>),
    py::arg("prec"), py::arg("out"), py::arg("c"), py::arg("tree_a"), py::arg("tree_b"), py::arg("maxIter") = -1,
    "Multiplies two function trees");

py::class_<DerivativeOperator<2>> deriv(m, "Derivative Operator");
deriv
    .def(py::init<MultiResolutionAnalysis<2>>());

py::class_<ABGVOperator<2>> (m, "ABGVOperator", deriv)
    .def(py::init< MultiResolutionAnalysis<2> &, double, double >());
}
