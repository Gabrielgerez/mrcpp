import numpy as np
import vampyr3d as vp

from math import isclose

min_scale = -4
max_depth = 25
order = 5
prec = 1e-3

corner = np.array([-1, -1, -1])
boxes = np.array([2, 2, 2])

world = vp.BoundingBox(min_scale, corner, boxes)

basis = vp.InterpolatingBasis(order)

MRA = vp.MultiResolutionAnalysis(world, basis, max_depth)


def phi_exact(x, y, z):
    beta = 100
    alpha = (beta/np.pi)**(3/2)

    return alpha*np.exp(-beta*(x**2 + y**2 + z**2))


def v_helm(x, y, z):
    mu = 10.0
    beta = 100.0
    alpha = (beta/np.pi)**(3/2)
    coef = -6.0*beta + 4*beta**2*x**2 +\
        4*beta**2*y**2 + 4*beta**2*z**2 - mu**2

    return (-1/(4.0*np.pi))*alpha*coef*np.exp(-beta*(x**2 + y**2 + z**2))


def v_pois(x, y, z):
    beta = 100.0
    alpha = (beta/np.pi)**(3/2)
    coef = -6.0*beta + 4*beta**2*x**2 +\
        4*beta**2*y**2 + 4*beta**2*z**2

    return (-1/(4.0*np.pi))*alpha*coef*np.exp(-beta*(x**2 + y**2 + z**2))


H = vp.HelmholtzOperator(MRA, 10.0, prec)
P = vp.PoissonOperator(MRA, prec)


phi_tree = vp.FunctionTree(MRA)
phi_tree_pois = vp.FunctionTree(MRA)
v_tree = vp.FunctionTree(MRA)
v_tree_pois = vp.FunctionTree(MRA)

add_tree = vp.FunctionTree(MRA)
add_vec_tree = vp.FunctionTree(MRA)
mult_vec_tree = vp.FunctionTree(MRA)


vp.project(prec, v_tree, v_helm)
vp.project(prec, v_tree_pois, v_pois)

vp.apply(prec, phi_tree, H, v_tree)
vp.apply(prec, phi_tree_pois, P, v_tree_pois)


def test_IsIntWorking():
    assert isclose(1.0, phi_tree.integrate(), rel_tol=prec)


def test_BBGetScale():
    assert world.getScale() == min_scale


def test_IBGetScalingOrder():
    assert basis.getScalingOrder() == order


def test_MRAGetOrder():
    assert MRA.getOrder() == order


def test_evalf_helm():
    assert isclose(phi_tree.evalf(0, 0, 0), phi_exact(0, 0, 0), rel_tol=prec)


def test_evalf_pelm():
    assert isclose(phi_tree_pois.evalf(0, 0, 0),
                   phi_exact(0, 0, 0), rel_tol=prec)


def test_add():
    vp.add(prec/10, add_tree, 1.0, phi_tree, -1, phi_tree_pois)
    assert isclose(add_tree.evalf(0, 0, 0), 0.0, abs_tol=prec*10)


def test_add_vec():
    sum_vec = vp.FunctionTreeVector()
    vp.push_back(sum_vec, 1.0, phi_tree)
    vp.push_back(sum_vec, -1.0, phi_tree)
    vp.add(prec/10, add_vec_tree, sum_vec)
    assert isclose(add_vec_tree.evalf(0.0, 0.0, 0.0), 0.0, abs_tol=prec*10)


def test_multiply():
    mult_tree = vp.FunctionTree(MRA)
    vp.multiply(prec, mult_tree, 1, phi_tree, phi_tree_pois)
    assert isclose(mult_tree.evalf(0, 0, 0),
                   phi_exact(0, 0, 0)**2, rel_tol=prec)


def test_multiply_vec():
    multiply_vec = vp.FunctionTreeVector()
    vp.push_back(multiply_vec, 1.0, phi_tree)
    vp.push_back(multiply_vec, 1.0, phi_tree)
    vp.multiply(prec/10, mult_vec_tree, multiply_vec)
    assert isclose(mult_vec_tree.evalf(0, 0, 0),
                   phi_exact(0, 0, 0)**2, rel_tol=prec)


def test_divergence_gradient():
    F_tree = vp.FunctionTree(MRA)
    df_tree = vp.FunctionTree(MRA)

    def F(x, y, z):
        return (x**4) + (y**4) + (z**4)

    vp.project(prec, F_tree, F)

    f_vector = vp.gradient(D, F_tree)
    vp.divergence(df_tree, D, f_vector)

    assert isclose(df_tree.evalf(1, 0, 0), 12, abs_tol=prec)
