/*
 * MRCPP, a numerical library based on multiresolution analysis and
 * the multiwavelet basis which provide low-scaling algorithms as well as
 * rigorous error control in numerical computations.
 * Copyright (C) 2020 Stig Rune Jensen, Jonas Juselius, Luca Frediani and contributors.
 *
 * This file is part of MRCPP.
 *
 * MRCPP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MRCPP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MRCPP.  If not, see <https://www.gnu.org/licenses/>.
 *
 * For information on the complete list of contributors to MRCPP, see:
 * <https://mrcpp.readthedocs.io/>
 */

#include "SerialTree.h"
#include "MWTree.h"
#include "utils/Printer.h"
#include "utils/math_utils.h"

using namespace Eigen;

namespace mrcpp {

template <int D>
SerialTree<D>::SerialTree(MWTree<D> *tree, SharedMemory *mem)
        : nNodes(0)
        , maxNodesPerChunk(0)
        , sizeNodeCoeff(0)
        , coeffStack(nullptr)
        , maxNodes(0)
        , tree_p(tree)
        ,
#ifdef HAVE_MPI
        shMem(mem) {
#else
        shMem(nullptr) {
#endif
}

/** Make children scaling coefficients from parent
 * Other node info are not used/set
 * coeff_in are not modified.
 * The output is written directly into the 8 children scaling coefficients.
 * NB: ASSUMES that the children coefficients are separated by Children_Stride!
 */
template <int D>
void SerialTree<D>::S_mwTransform(double *coeff_in,
                                  double *coeff_out,
                                  bool readOnlyScaling,
                                  int stride,
                                  bool b_overwrite) {
    int operation = Reconstruction;
    int kp1 = this->getTree()->getKp1();
    int kp1_d = this->getTree()->getKp1_d();
    int tDim = (1 << D);
    int kp1_dm1 = math_utils::ipow(kp1, D - 1);
    const MWFilter &filter = this->getTree()->getMRA().getFilter();
    double overwrite = 0.0;
    double tmpcoeff[kp1_d * tDim];
    double tmpcoeff2[kp1_d * tDim];
    int ftlim = tDim;
    int ftlim2 = tDim;
    int ftlim3 = tDim;
    if (readOnlyScaling) {
        ftlim = 1;
        ftlim2 = 2;
        ftlim3 = 4;
        // NB: Careful: tmpcoeff tmpcoeff2 are not initialized to zero
        // must not read these unitialized values!
    }

    overwrite = 0.0;
    int i = 0;
    int mask = 1;
    for (int gt = 0; gt < tDim; gt++) {
        double *out = tmpcoeff + gt * kp1_d;
        for (int ft = 0; ft < ftlim; ft++) {
            // Operate in direction i only if the bits along other
            // directions are identical. The bit of the direction we
            // operate on determines the appropriate filter/operator
            if ((gt | mask) == (ft | mask)) {
                double *in = coeff_in + ft * kp1_d;
                int filter_index = 2 * ((gt >> i) & 1) + ((ft >> i) & 1);
                const MatrixXd &oper = filter.getSubFilter(filter_index, operation);

                math_utils::apply_filter(out, in, oper, kp1, kp1_dm1, overwrite);
                overwrite = 1.0;
            }
        }
        overwrite = 0.0;
    }
    if (D > 1) {
        i++;
        mask = 2; // 1 << i;
        for (int gt = 0; gt < tDim; gt++) {
            double *out = tmpcoeff2 + gt * kp1_d;
            for (int ft = 0; ft < ftlim2; ft++) {
                // Operate in direction i only if the bits along other
                // directions are identical. The bit of the direction we
                // operate on determines the appropriate filter/operator
                if ((gt | mask) == (ft | mask)) {
                    double *in = tmpcoeff + ft * kp1_d;
                    int filter_index = 2 * ((gt >> i) & 1) + ((ft >> i) & 1);
                    const MatrixXd &oper = filter.getSubFilter(filter_index, operation);

                    math_utils::apply_filter(out, in, oper, kp1, kp1_dm1, overwrite);
                    overwrite = 1.0;
                }
            }
            overwrite = 0.0;
        }
    }
    if (D > 2) {
        overwrite = 1.0;
        if (b_overwrite) overwrite = 0.0;
        i++;
        mask = 4; // 1 << i;
        for (int gt = 0; gt < tDim; gt++) {
            double *out = coeff_out + gt * stride; // write right into children
            for (int ft = 0; ft < ftlim3; ft++) {
                // Operate in direction i only if the bits along other
                // directions are identical. The bit of the direction we
                // operate on determines the appropriate filter/operator
                if ((gt | mask) == (ft | mask)) {
                    double *in = tmpcoeff2 + ft * kp1_d;
                    int filter_index = 2 * ((gt >> i) & 1) + ((ft >> i) & 1);
                    const MatrixXd &oper = filter.getSubFilter(filter_index, operation);

                    math_utils::apply_filter(out, in, oper, kp1, kp1_dm1, overwrite);
                    overwrite = 1.0;
                }
            }
            overwrite = 1.0;
            if (b_overwrite) overwrite = 0.0;
        }
    }

    if (D > 3) MSG_ABORT("D>3 NOT IMPLEMENTED for S_mwtransform");

    if (D < 3) {
        double *out;
        if (D == 1) out = tmpcoeff;
        if (D == 2) out = tmpcoeff2;
        if (b_overwrite) {
            for (int j = 0; j < tDim; j++) {
                for (int i = 0; i < kp1_d; i++) { coeff_out[i + j * stride] = out[i + j * kp1_d]; }
            }
        } else {
            for (int j = 0; j < tDim; j++) {
                for (int i = 0; i < kp1_d; i++) { coeff_out[i + j * stride] += out[i + j * kp1_d]; }
            }
        }
    }
}

// Specialized for D=3 below.
template <int D> void SerialTree<D>::S_mwTransformBack(double *coeff_in, double *coeff_out, int stride) {
    NOT_IMPLEMENTED_ABORT;
}

/** Make parent from children scaling coefficients
 * Other node info are not used/set
 * coeff_in are not modified.
 * The output is read directly from the 8 children scaling coefficients.
 * NB: ASSUMES that the children coefficients are separated by Children_Stride!
 */
template <> void SerialTree<3>::S_mwTransformBack(double *coeff_in, double *coeff_out, int stride) {
    int operation = Compression;
    int kp1 = this->getTree()->getKp1();
    int kp1_d = this->getTree()->getKp1_d();
    int tDim = 8;
    int kp1_dm1 = math_utils::ipow(kp1, 2);
    const MWFilter &filter = this->getTree()->getMRA().getFilter();
    double overwrite = 0.0;
    double tmpcoeff[kp1_d * tDim];

    int ftlim = tDim;
    int ftlim2 = tDim;
    int ftlim3 = tDim;

    int i = 0;
    int mask = 1;
    for (int gt = 0; gt < tDim; gt++) {
        double *out = coeff_out + gt * kp1_d;
        for (int ft = 0; ft < ftlim; ft++) {
            // Operate in direction i only if the bits along other
            // directions are identical. The bit of the direction we
            // operate on determines the appropriate filter/operator
            if ((gt | mask) == (ft | mask)) {
                double *in = coeff_in + ft * stride;
                int filter_index = 2 * ((gt >> i) & 1) + ((ft >> i) & 1);
                const MatrixXd &oper = filter.getSubFilter(filter_index, operation);

                math_utils::apply_filter(out, in, oper, kp1, kp1_dm1, overwrite);
                overwrite = 1.0;
            }
        }
        overwrite = 0.0;
    }
    i++;
    mask = 2; // 1 << i;
    for (int gt = 0; gt < tDim; gt++) {
        double *out = tmpcoeff + gt * kp1_d;
        for (int ft = 0; ft < ftlim2; ft++) {
            // Operate in direction i only if the bits along other
            // directions are identical. The bit of the direction we
            // operate on determines the appropriate filter/operator
            if ((gt | mask) == (ft | mask)) {
                double *in = coeff_out + ft * kp1_d;
                int filter_index = 2 * ((gt >> i) & 1) + ((ft >> i) & 1);
                const MatrixXd &oper = filter.getSubFilter(filter_index, operation);

                math_utils::apply_filter(out, in, oper, kp1, kp1_dm1, overwrite);
                overwrite = 1.0;
            }
        }
        overwrite = 0.0;
    }
    i++;
    mask = 4; // 1 << i;
    for (int gt = 0; gt < tDim; gt++) {
        double *out = coeff_out + gt * kp1_d;
        // double *out = coeff_out + gt * N_coeff;
        for (int ft = 0; ft < ftlim3; ft++) {
            // Operate in direction i only if the bits along other
            // directions are identical. The bit of the direction we
            // operate on determines the appropriate filter/operator
            if ((gt | mask) == (ft | mask)) {
                double *in = tmpcoeff + ft * kp1_d;
                int filter_index = 2 * ((gt >> i) & 1) + ((ft >> i) & 1);
                const MatrixXd &oper = filter.getSubFilter(filter_index, operation);

                math_utils::apply_filter(out, in, oper, kp1, kp1_dm1, overwrite);
                overwrite = 1.0;
            }
        }
        overwrite = 0.0;
    }
}

template class SerialTree<1>;
template class SerialTree<2>;
template class SerialTree<3>;

} // namespace mrcpp
