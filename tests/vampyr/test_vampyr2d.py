import numpy as np
import vampyr2d as vp

from math import isclose

min_scale = -4
max_depth = 25
order = 7
prec = 1e-5

corner = np.array([-1, -1])
boxes = np.array([2, 2])

world = vp.BoundingBox(min_scale, corner, boxes)

basis = vp.InterpolatingBasis(order)

MRA = vp.MultiResolutionAnalysis(world, basis, max_depth)


def phi(x, y):
    beta = 100
    alpha = (beta/np.pi)**(2/2)

    return alpha*np.exp(-beta*(x**2 + y**2))


phi_tree = vp.FunctionTree(MRA)
add_tree = vp.FunctionTree(MRA)
mult_tree = vp.FunctionTree(MRA)


vp.project(prec, phi_tree, phi)


def test_IsIntWorking():
    assert isclose(1.0, phi_tree.integrate(), rel_tol=prec)


def test_BBGetScale():
    assert world.getScale() == min_scale


def test_IBGetScalingOrder():
    assert basis.getScalingOrder() == order


def test_MRAGetOrder():
    assert MRA.getOrder() == order


def test_add():
    vp.add(prec/10, add_tree, 1.0, phi_tree, -1, phi_tree)
    assert isclose(add_tree.evalf(0, 0), 0.0, abs_tol=prec*10)


def test_multiply():
    vp.multiply(prec, mult_tree, 1, phi_tree, phi_tree)
    assert isclose(mult_tree.evalf(0, 0), phi(0, 0)**2, rel_tol=prec)
