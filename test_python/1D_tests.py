import numpy as np
from pymrcpp import *
min_scale = -4
max_depth = 25
order = 7
prec = 1e-5
corner = np.array([0])
boxes = np.array([1])

world = BoundingBox1D(min_scale, corner, boxes)
basis = InterpolatingBasis(order)
MRA = MultiResolutionAnalysis1D(world, basis, max_depth)

def f(x):
    return np.exp(-x**2)

def df(x):
    return -2*x*np.exp(-x**2)


D = ABGVOperator1D(MRA, 0.0, 0.0)
f_tree = FunctionTree1D(MRA)
df_tree = FunctionTree1D(MRA)
dg_tree = FunctionTree1D(MRA)
err_tree = FunctionTree1D(MRA)

project(prec, f_tree, f, -1)
project(prec, df_tree, df, -1)
apply(dg_tree, D, f_tree, 0)

add(prec, err_tree, 1.0, df_tree, -1.0, dg_tree, -1)

def test_add():
    D = ABGVOperator1D(MRA, 0.0, 0.0)
    f_tree = FunctionTree1D(MRA)
    df_tree = FunctionTree1D(MRA)
    dg_tree = FunctionTree1D(MRA)
    err_tree = FunctionTree1D(MRA)

    project(prec, f_tree, f, -1)
    project(prec, df_tree, df, -1)
    apply(dg_tree, D, f_tree, 0)

    add(prec, err_tree, 1.0, df_tree, -1.0, dg_tree, -1)
    if np.abs(np.sqrt(err_tree.getSquareNorm())) < prec:
        assert True
    else:
        assert False