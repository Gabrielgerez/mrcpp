#
# \date Mar 08, 2018
# \author Magnar Bjørgve <magnar.bjorgve@uit.no> \n
#         Hylleraas Centre for Quantum Molecular Sciences \n
#         UiT - The Arctic University of Norway
#

import vampyr3d as vp
import numpy as np
from scipy.special import erf

prec = 1.0e-3

# Constructing world box
min_scale = -4
corner = np.array([-1, -1, -1])
boxes = np.array([2, 2, 2])
world = vp.BoundingBox(min_scale, corner, boxes)


def setup_potential(V, Z):

    def f(x, y, z):
        c = 0.00435*prec/(Z**5)

        def u(r):
            tmp = 3.0*np.sqrt(np.pi)
            return erf(r)/r + 1.0/tmp*(np.exp(-r**2) + 16.0*np.exp(-4.0*r**2))

        r = np.sqrt(x**2 + y**2 + z**2)
        return -1.0*Z*u(r/c)/c

    vp.project(prec, V, f)


def setup_initial_guess(phi):

    def f(x, y, z):
        r = np.sqrt(x**2 + y**2 + z**2)
        return 1.0*np.exp(-1.0*r**2)

    vp.project(prec, phi, f)
    phi.normalize()


# Constructing basis and MRA
order = 5
basis = vp.InterpolatingBasis(order)
MRA = vp.MultiResolutionAnalysis(world, basis, 25)

# Nuclear potential
Z = 1.0
V = vp.FunctionTree(MRA)
setup_potential(V, Z)


phi_n = vp.FunctionTree(MRA)
setup_initial_guess(phi_n)

epsilon_n = -0.5
it = 1
error = 1.0

while error > 10*prec:

    if (epsilon_n > 0.0):
        epsilon_n = -1.0*epsilon_n

    # Initalize the Helmholtz Operator
    mu_n = np.sqrt(-2.0*epsilon_n)
    H = vp.HelmholtzOperator(MRA, mu_n, prec)

    # Compute Helmholtz argument V*phi
    Vphi = vp.FunctionTree(MRA)
    vp.multiply(prec, Vphi, 1.0, V, phi_n)

    # Apply Helmholtz operator phi^n+1 = H[V*phi^n]
    phi_np1 = vp.FunctionTree(MRA)
    vp.apply(prec, phi_np1, H, Vphi)
    phi_np1.rescale(-1.0/(2.0*np.pi))

    # Compute orbital residual
    d_phi_n = vp.FunctionTree(MRA)
    vp.add(prec, d_phi_n, 1.0, phi_np1, -1.0, phi_n)
    error = np.sqrt(d_phi_n.getSquareNorm())

    # Compute energy update <Vphi|d_phi>/||phi||
    d_epsilon_n = vp.dot(Vphi, d_phi_n)/phi_np1.getSquareNorm()
    epsilon_np1 = epsilon_n + d_epsilon_n

    print("Iteration:", it, "Energy:",
          epsilon_np1, "Norm:", phi_np1.getSquareNorm(),
          "Error:", error)

    # Prepare for next iteration
    epsilon_n = epsilon_np1
    phi_n = phi_np1
    phi_n.normalize()
    it = it+1
