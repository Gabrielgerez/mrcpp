#include "catch.hpp"

#include "factory_functions.h"

#include "functions/GaussPoly.h"
#include "treebuilders/WaveletAdaptor.h"
#include "treebuilders/project.h"
#include "treebuilders/grid.h"
#include "treebuilders/multiply.h"

using namespace mrcpp;

namespace multiplication {

template<int D> void testMultiplication();

SCENARIO("Multiplying MW trees", "[multiplication], [tree_builder]") {
    GIVEN("Two MW functions in 1D") {
        testMultiplication<1>();
    }
    GIVEN("Two MW functions in 2D") {
        testMultiplication<2>();
    }
    GIVEN("Two MW functions in 3D") {
        testMultiplication<3>();
    }
}

template<int D> void testMultiplication() {
    const double prec = 1.0e-4;

    double alpha = 1.0;
    double beta_a = 110.0;
    double beta_b = 50.0;
    double pos_a[3] = {-0.25, 0.35, 1.05};
    double pos_b[3] = {-0.20, 0.50, 1.05};

    GaussFunc<D> a_func(beta_a, alpha, pos_a);
    GaussFunc<D> b_func(beta_b, alpha, pos_b);
    GaussPoly<D> ref_func = a_func*b_func;

    MultiResolutionAnalysis<D> *mra = 0;
    initialize(&mra);

    // Initialize trees
    FunctionTree<D> a_tree(*mra);
    FunctionTree<D> b_tree(*mra);
    FunctionTree<D> ref_tree(*mra);

    // Build empty grids
    build_grid(a_tree, a_func);
    build_grid(b_tree, b_func);
    build_grid(ref_tree, ref_func);

    // Project functions
    project(prec, a_tree, a_func);
    project(prec, b_tree, b_func);
    project(prec, ref_tree, ref_func);

    const double ref_int = ref_tree.integrate();
    const double ref_norm = ref_tree.getSquareNorm();

    FunctionTreeVector<D> prod_vec;
    WHEN("the functions are multiplied") {
        FunctionTree<D> c_tree(*mra);
        prod_vec.push_back(std::make_tuple(1.0, &a_tree));
        prod_vec.push_back(std::make_tuple(1.0, &b_tree));
        multiply(prec, c_tree, prod_vec);
        prod_vec.clear();

        THEN("the MW product equals the analytic product") {
            double c_int = c_tree.integrate();
            double c_dot = dot(c_tree, ref_tree);
            double c_norm = c_tree.getSquareNorm();
            REQUIRE( c_int == Approx(ref_int) );
            REQUIRE( c_dot == Approx(ref_norm) );
            REQUIRE( c_norm == Approx(ref_norm) );
        }
    }
    finalize(&mra);
}

TEST_CASE("Dot product FunctionTreeVectors", "[multiplication], [tree_vector_dot]") {
    MultiResolutionAnalysis<3> *mra = 0;
    initialize(&mra);

    double prec = 1.0e-4;

    auto fx = [] (const double *r) -> double {
        double r2 = (r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
        return r[1]*r[2]*std::exp(-1.0*r2);
    };
    auto fy = [] (const double *r) -> double {
        double r2 = (r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
        return r[0]*r[2]*std::exp(-1.5*r2);
    };
    auto fz = [] (const double *r) -> double {
        double r2 = (r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
        return r[0]*r[1]*std::exp(-2.0*r2);
    };

    FunctionTree<3> fx_tree(*mra);
    FunctionTree<3> fy_tree(*mra);
    FunctionTree<3> fz_tree(*mra);

    project(prec, fx_tree, fx);
    project(prec, fy_tree, fy);
    project(prec, fz_tree, fz);

    FunctionTreeVector<3> vec_a;
    vec_a.push_back(std::make_tuple(1.0, &fx_tree));
    vec_a.push_back(std::make_tuple(2.0, &fy_tree));
    vec_a.push_back(std::make_tuple(3.0, &fz_tree));

    FunctionTreeVector<3> vec_b;
    vec_b.push_back(std::make_tuple(1.0, &fz_tree));
    vec_b.push_back(std::make_tuple(2.0, &fy_tree));
    vec_b.push_back(std::make_tuple(3.0, &fx_tree));

    FunctionTree<3> dot_ab(*mra);
    build_grid(dot_ab, vec_a);
    build_grid(dot_ab, vec_b);
    dot(0.1*prec, dot_ab, vec_a, vec_b);

    for (int i = 0; i < 10; i++) {
        const double r[3] = {-0.4 + 0.01*i, 0.9 - 0.05*i, 0.7 + 0.1*i};
        const double ref = 1.0*fx(r)*fz(r) + 4.0*fy(r)*fy(r) + 9.0*fz(r)*fx(r);
        REQUIRE( dot_ab.evalf(r) == Approx(ref).epsilon(prec) );
    }

    finalize(&mra);
}

} // namespace
