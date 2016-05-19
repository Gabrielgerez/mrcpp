#ifndef MWOPERATOR_H
#define MWOPERATOR_H

#include "TreeBuilder.h"
#include "OperatorTreeVector.h"

template<int D>
class MWOperator : public TreeBuilder<D> {
public:
    MWOperator(const MultiResolutionAnalysis<D> &mra, double pr)
        : TreeBuilder<D>(mra),
          apply_prec(pr) {
    }

    virtual ~MWOperator() {
    }

    void setPrecision(double pr) { this->apply_prec = pr; }
    void multPrecision(double fac) { this->apply_prec *= fac; }

    FunctionTree<D> *operator()(FunctionTree<D> &inp);
    void operator()(FunctionTree<D> &out, FunctionTree<D> &inp, int maxIter = -1);

protected:
    double apply_prec;
    OperatorTreeVector oper;

    void clearOperator();
    MultiResolutionAnalysis<2> *getOperatorMRA();
};

#endif // MWOPERATOR_H