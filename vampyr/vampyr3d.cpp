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
#include "operators/PoissonOperator.h"
#include "operators/HelmholtzOperator.h"

#include "treebuilders/add.h"
#include "treebuilders/multiply.h"
#include "treebuilders/apply.h"
#include "treebuilders/project.h"



using namespace mrcpp;
namespace py = pybind11;

void bases(py::module &);

PYBIND11_MODULE(vampyr3d, m) {

m.def("project", py::overload_cast<double, FunctionTree<3> &, std::function<double (double, double, double)>, int>(&project3D),
    py::arg("prec"), py::arg("out"), py::arg("func"), py::arg("maxIter")= -1);

py::class_<PyBoundingBox<3>> (m, "BoundingBox3D")
.def(py::init<int, py::array_t<int>, py::array_t <int>>())
.def(py::init<int, int *,  int *>()) //1D cases can be initialized without array type input
.def("getScale", &PyBoundingBox<3>::getScale);

py::class_<MultiResolutionAnalysis<3>> (m, "MultiResolutionAnalysis3D")
.def(py::init<PyBoundingBox<3>, ScalingBasis, int>())
.def("getOrder", &MultiResolutionAnalysis<3>::getOrder)
.def("getMaxDepth", &MultiResolutionAnalysis<3>::getMaxDepth)
.def("getMaxScale", &MultiResolutionAnalysis<3>::getMaxScale);

py::class_<MWTree<3>> mwtree(m, "MWTree");
mwtree
.def(py::init<MultiResolutionAnalysis<3>>())
.def("getSquareNorm", &MWTree<3>::getSquareNorm);

py::class_<FunctionTree<3>> functree(m, "FunctionTree3D", mwtree);
functree
.def(py::init<MultiResolutionAnalysis<3>>())
.def("integrate", &FunctionTree<3>::integrate)
.def("clear", &FunctionTree<3>::clear)
.def("normalize", &FunctionTree<3>::normalize)
.def("evalf", py::overload_cast<double, double, double>(&FunctionTree<3>::evalf));



m.def("apply", py::overload_cast<double, FunctionTree<3> &, ConvolutionOperator<3> &, FunctionTree<3> &, int>(&apply<3>),
        py::arg("prec"), py::arg("out"), py::arg("oper"), py::arg("inp"), py::arg("maxIter") = -1);
m.def("apply", py::overload_cast<FunctionTree<3> &, DerivativeOperator<3> &, FunctionTree<3> &, int>(&apply<3>));

bases(m);


m.def("add", py::overload_cast<double, FunctionTree<3> &, double, FunctionTree<3> &, double, FunctionTree<3> &, int>(&add<3>),
    py::arg("prec"), py::arg("out"), py::arg("a"),  py::arg("tree_a"), py::arg("b"), py::arg("tree_b"), py::arg("maxIter") = -1,
    "Adds to function trees");

m.def("project", py::overload_cast<double, FunctionTree<3> &, RepresentableFunction<3> &, int>(&project<3>),
    py::arg("prec"), py::arg("out"), py::arg("inp"), py::arg("maxIter")= -1);

m.def("multiply", py::overload_cast<double, FunctionTree<3> &, double, FunctionTree<3> &, FunctionTree<3> &, int >(&multiply<3>),
        py::arg("prec"), py::arg("out"), py::arg("c"), py::arg("tree_a"), py::arg("tree_b"), py::arg("maxIter") = -1,
        "Multiplies two function trees");

py::class_<DerivativeOperator<3>> deriv(m, "Derivative Operator");
deriv
.def(py::init<MultiResolutionAnalysis<3>>());

py::class_<ABGVOperator<3>> (m, "ABGVOperator", deriv)
.def(py::init< MultiResolutionAnalysis<3> &, double, double >());


py::class_<ConvolutionOperator<3>> convop(m, "ConvolutionOperator");
convop
.def(py::init<MultiResolutionAnalysis<3> &, double>());

py::class_<PoissonOperator> (m, "PoissonOperator", convop)
.def(py::init<MultiResolutionAnalysis<3> &, double >());

py::class_<HelmholtzOperator> (m, "HelmholtzOperator", convop)
.def(py::init<MultiResolutionAnalysis<3> &, double, double>());

}
