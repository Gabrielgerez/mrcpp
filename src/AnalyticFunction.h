#pragma once

#include "RepresentableFunction.h"

//#include "pybind11/include/pybind11/pybind11.h"
//#include "pybind11/include/pybind11/numpy.h"
namespace mrcpp {

template<int D>
class AnalyticFunction : public RepresentableFunction<D> {
public:
    AnalyticFunction(std::function<double (const double *r)> f,
                     const double *a = 0,
                     const double *b = 0)
            : RepresentableFunction<D>(a, b),
              func(f) { }
    virtual ~AnalyticFunction() { }

    virtual double evalf(const double *r) const {
        double val = 0.0;
        if (not this->outOfBounds(r)) val = this->func(r);
        return val;
    }
protected:
    std::function<double (const double *r)> func;
};


class AnalyticFunction3D : public RepresentableFunction<3> {
public:
    AnalyticFunction3D(std::function<double (double x, double y, double z)> f)
         : func(f) { }

    virtual double evalf(const double *r) const {
        double val = 0.0;
        if (not this->outOfBounds(r)) val = this->func(r[0], r[1], r[2]);
        return val;
    }
protected:
    std::function<double (double x, double y, double z)> func;
};

class AnalyticFunction2D : public RepresentableFunction<2> {
public:
    AnalyticFunction2D(std::function<double (double x, double y)> f)
            : func(f) { }

    virtual double evalf(const double *r) const {
        double val = 0.0;
        if (not this->outOfBounds(r)) val = this->func(r[0], r[1]);
        return val;
    }
protected:
    std::function<double (double x, double y)> func;
};

class AnalyticFunction1D : public RepresentableFunction<1> {
public:
    AnalyticFunction1D(std::function<double (double x)> f)
            : func(f) { }

    virtual double evalf(const double *r) const {
        double val = 0.0;
        if (not this->outOfBounds(r)) val = this->func(r[0]);
        return val;
    }
protected:
    std::function<double (double x)> func;
};

}
