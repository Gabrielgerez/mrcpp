#ifndef FACTORY_FUNCTIONS_H
#define FACTORY_FUNCTIONS_H

#include "MRCPP/mrcpp_declarations.h"
#include "core/InterpolatingBasis.h"
#include "core/LegendreBasis.h"
#include "functions/GaussFunc.h"
#include "trees/BoundingBox.h"
#include "trees/FunctionTree.h"
#include "trees/FunctionTreeVector.h"
#include "trees/MultiResolutionAnalysis.h"
#include "trees/NodeIndex.h"
#include "utils/Printer.h"
#include "utils/details.h"

template <class T> void finalize(T **obj) {
    if (obj == nullptr) INVALID_ARG_ABORT;
    if (*obj == nullptr) INVALID_ARG_ABORT;
    delete *obj;
    *obj = nullptr;
}

template <int D> void initialize(mrcpp::NodeIndex<D> **idx) {
    if (idx == nullptr) INVALID_ARG_ABORT;
    if (*idx != nullptr) INVALID_ARG_ABORT;
    int scale = 1;
    int l[D];
    for (int d = 0; d < D; d++) { l[d] = d - 1; }
    *idx = new mrcpp::NodeIndex<D>(scale, l);
}

template <int D> void testInitial(const mrcpp::NodeIndex<D> *idx) {
    if (idx == nullptr) INVALID_ARG_ABORT;

    const int scale = 1;
    REQUIRE((scale == idx->getScale()));

    for (int d = 0; d < D; d++) {
        const int l = d - 1;
        REQUIRE((l == idx->getTranslation(d)));
        REQUIRE((l == idx->getTranslation()[d]));
    }
}

template <int D>
void initialize(mrcpp::BoundingBox<D> **box, bool periodic = false, const std::array<double, D> &period = {}) {
    if (box == nullptr) INVALID_ARG_ABORT;
    if (*box != nullptr) INVALID_ARG_ABORT;

    if (not periodic) {
        std::array<int, D> nb;
        for (int d = 0; d < D; d++) nb[d] = d + 1;
        mrcpp::NodeIndex<D> *nIdx = nullptr;
        initialize<D>(&nIdx);
        *box = new mrcpp::BoundingBox<D>(*nIdx, nb);
        finalize(&nIdx);
    } else {
        *box = new mrcpp::BoundingBox<D>(period);
    }
}

template <int D> void testInitial(const mrcpp::BoundingBox<D> *box) {
    if (box == nullptr) INVALID_ARG_ABORT;

    const mrcpp::NodeIndex<D> &cIdx = box->getCornerIndex();
    testInitial<D>(&cIdx);

    for (int d = 0; d < D; d++) {
        REQUIRE((box->getUnitLength(d) > 0.0));
        REQUIRE((box->getUnitLengths()[d] > 0.0));

        REQUIRE((box->getBoxLength(d) > 0.0));
        REQUIRE((box->getBoxLengths()[d] > 0.0));

        REQUIRE((box->getLowerBound(d) < box->getUpperBound(d)));
        REQUIRE((box->getLowerBounds()[d] < box->getUpperBounds()[d]));
    }

    int tot_boxes = 1;
    for (int d = 0; d < D; d++) {
        const int nb = d + 1;
        REQUIRE((box->size(d) == nb));
        tot_boxes *= box->size(d);
    }
    REQUIRE((box->size() == tot_boxes));
}

template <int D>
void initialize(mrcpp::MultiResolutionAnalysis<D> **mra,
                bool periodic = false,
                const std::array<double, D> &period = {}) {
    if (mra == nullptr) INVALID_ARG_ABORT;
    if (*mra != nullptr) INVALID_ARG_ABORT;

    int k = 5;
    mrcpp::InterpolatingBasis basis(k);
    mrcpp::BoundingBox<D> *world = nullptr;
    initialize<D>(&world, periodic, period);
    *mra = new mrcpp::MultiResolutionAnalysis<D>(*world, basis);
    finalize(&world);
}

/* Initializing a D-dimensional Gaussian of unit charge */
template <int D>
void initialize(mrcpp::GaussFunc<D> **func, bool periodic = false, const std::array<double, D> &period = {}) {
    double beta = 1.0e4;
    double alpha = std::pow(beta / mrcpp::pi, D / 2.0);
    double pos_data[3] = {-0.2, 0.5, 1.0};
    const auto pow = std::array<int, D>{};

    auto pos = mrcpp::details::convert_to_std_array<double, D>(pos_data);

    *func = new mrcpp::GaussFunc<D>(beta, alpha, pos);
    if (periodic) (*func)->makePeriodic(period);
}

#endif // FACTORY_FUNCTIONS_H
