import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import matplotlib.patches as patches
from matplotlib.lines import Line2D

from parameters import *



#--------------------------------#
# Public functions               #
#--------------------------------#


def calcTM (
    tm: pd.DataFrame, 
    tau: float=otmTau, 
    sample_size: int=0, 
    rng: np.random.Generator|None=None
) -> tuple[pd.DataFrame, pd.DataFrame]:
    '''
    Generate electron and positron decay products from a true-muonium sample.

    Samples the TM decay for the specified lifetime, calculates the decay-product
    kinematics and detector hit positions, and applies the basic detector
    acceptance masks. If requested, the input sample is resampled to a given size.
    '''
    
    # create random-number generator if not provided
    rng = np.random.default_rng() if rng is None else rng
    
    # resample events with replacement
    if sample_size:
        if sample_size>len(tm):
            print(f'more elements selected than in dataset')
        tm = tm.sample(sample_size, replace=True, random_state=rng)
    
    # sample the TM decay, this gives the columns [Etot, px, py, pz]
    ele, pos = _tmDecay(tm, tau, rng)

    # transverse momentum
    ele['pt'] = np.sqrt(ele.px**2 + ele.py**2)
    pos['pt'] = np.sqrt(pos.px**2 + pos.py**2)

    # total momentum
    ele['ptot'] = np.sqrt(ele.px**2 + ele.py**2 + ele.pz**2)
    pos['ptot'] = np.sqrt(pos.px**2 + pos.py**2 + pos.pz**2)
    
    # polar angles with respect to z-axis
    ele['theta'] = np.arccos(ele.pz / ele.ptot)
    pos['theta'] = np.arccos(pos.pz / pos.ptot)
    
    # azimuthal angle (with respect to the x-axis)
    ele['phi'] = np.arctan2(ele['py'], ele['px'])
    pos['phi'] = np.arctan2(pos['py'], pos['px'])

    # calculate the hit positions in [mm]
    ele['hitX'], ele['hitY'], ele['hitZ'] = _forward_intersection(ele, detectorRadius).T
    pos['hitX'], pos['hitY'], pos['hitZ'] = _forward_intersection(pos, detectorRadius).T
    
    # mask for energy threshold
    ele['maskE'] = (ele.Etot>E_th_min) & (ele.Etot<E_th_max)
    pos['maskE'] = (pos.Etot>E_th_min) & (pos.Etot<E_th_max)
    
    # mask for longitudinal position cut
    ele['maskZ'] = (ele.hitZ>z_th_min) & (ele.hitZ<z_th_max)
    pos['maskZ'] = (pos.hitZ>z_th_min) & (pos.hitZ<z_th_max)

    return ele, pos


def combinatorialAnalysis(
    ele: pd.DataFrame, 
    pos: pd.DataFrame, 
    which: str='alpha'
) -> np.ndarray:
    '''
    Calculate pairwise observables for all possible electron--positron combinations.

    Depending on `which`, returns the opening angle ('alpha'), azimuthal-angle
    difference ('dPhi'), transverse momentum ('pt'), or invariant mass ('mInv')
    for every possible electron--positron pairing.
    '''
    
    # Convert relevant columns to NumPy arrays
    pos_px = pos['px'].to_numpy()[:, np.newaxis]  # shape (N, 1)
    pos_py = pos['py'].to_numpy()[:, np.newaxis]
    pos_pz = pos['pz'].to_numpy()[:, np.newaxis]
    pos_ptot = pos['ptot'].to_numpy()[:, np.newaxis]
    pos_Etot = pos['Etot'].to_numpy()[:, np.newaxis]

    ele_px = ele['px'].to_numpy()[np.newaxis, :]  # shape (1, M)
    ele_py = ele['py'].to_numpy()[np.newaxis, :]
    ele_pz = ele['pz'].to_numpy()[np.newaxis, :]
    ele_ptot = ele['ptot'].to_numpy()[np.newaxis, :]
    ele_Etot = ele['Etot'].to_numpy()[np.newaxis, :]

    # calculate the opening angle of all e+e- pairs in [rad]
    if which=='alpha':
        dot_product = pos_px * ele_px + pos_py * ele_py + pos_pz * ele_pz
        magnitudes = pos_ptot * ele_ptot        
        cos_alpha = np.clip(dot_product / magnitudes, -1.0, 1.0)
        alphaMat = np.arccos(cos_alpha)
        return alphaMat

    # calculate the azimuthal angle difference of all e+e- pairs in [rad]
    elif which=='dPhi':
        phi_ele = np.arctan2(ele['py'].to_numpy(), ele['px'].to_numpy())  # shape (M,)
        phi_pos = np.arctan2(pos['py'].to_numpy(), pos['px'].to_numpy())  # shape (N,)
        dPhi = phi_ele[np.newaxis, :] - phi_pos[:, np.newaxis] # shape (N, M)
        return (dPhi+np.pi) % (2*np.pi)

    # calculate the transverse momenta of all e+e- pairs in [MeV]
    elif which=='pt':
        ptMat = np.sqrt((pos_px + ele_px)**2 + (pos_py + ele_py)**2)
        return ptMat

    # calculate the invariant mass
    elif which=='mInv':
        mInv = np.sqrt((pos_Etot+ele_Etot)**2 - ((pos_px + ele_px)**2 + (pos_py + ele_py)**2 + (pos_pz + ele_pz)**2))
        return mInv

    else:
        raise ValueError(f'method "{which}" not implemented')


def bin_range(
    edges: np.ndarray, 
    low: float|None=None, 
    high: float|None=None, 
    inclusive_high: bool=False
) -> slice:
    '''
    Return a slice selecting histogram bins overlapping a specified interval.

    The selected interval is [low, high) by default, or [low, high] when
    inclusive_high is True. Either boundary may be omitted.
    '''
    n = len(edges) - 1
    if low is None:
        i0 = 0
    else:
        i0 = np.searchsorted(edges, low, side="right") - 1

    if high is None:
        i1 = n
    else:
        side = "right" if inclusive_high else "left"
        i1 = np.searchsorted(edges, high, side=side)

    i0 = max(0, min(n, i0))
    i1 = max(0, min(n, i1))
    
    return slice(i0, i1)


def add_hatched_exclusion(
    ax: plt.Axes, 
    x0: float, 
    x1: float, 
    y0: float, 
    y1: float,
    hatch: str='///', 
    color: str='C5', 
    zorder: int=0, 
    label: str|None=None
) -> None:
    '''
    Hatch the plotting region outside a specified rectangular acceptance area.

    The rectangle defined by x0, x1, y0, and y1 remains unshaded, while the
    surrounding region is indicated by a hatched exclusion pattern.
    '''

    # Current plot limits
    xmin, xmax = ax.get_xlim()
    ymin, ymax = ax.get_ylim()

    # pseudo-patch for label
    ax.add_patch(patches.Rectangle((np.nan, np.nan), np.nan, np.nan, facecolor='none', hatch=hatch, edgecolor=color, alpha=0.4, label=label))

    # Top
    ax.add_patch(patches.Rectangle((xmin, y1), xmax - xmin, ymax - y1, color=color, alpha=0.02, zorder=zorder))
    ax.add_patch(patches.Rectangle((xmin, y1), xmax - xmin, ymax - y1, facecolor='none', hatch=hatch, edgecolor=color, lw=0, alpha=0.4))

    # Bottom
    ax.add_patch(patches.Rectangle((xmin, ymin), xmax - xmin, y0 - ymin, color=color, alpha=0.02, zorder=zorder))
    ax.add_patch(patches.Rectangle((xmin, ymin), xmax - xmin, y0 - ymin, facecolor='none', hatch=hatch, edgecolor=color, lw=0, alpha=0.4))

    # Left
    ax.add_patch(patches.Rectangle((xmin, y0), x0 - xmin, y1 - y0, color=color, alpha=0.02, zorder=zorder))
    ax.add_patch(patches.Rectangle((xmin, y0), x0 - xmin, y1 - y0, facecolor='none', hatch=hatch, edgecolor=color, lw=0, alpha=0.4))

    # Right
    ax.add_patch(patches.Rectangle((x1, y0), xmax - x1, y1 - y0, color=color, alpha=0.02, zorder=zorder))
    ax.add_patch(patches.Rectangle((x1, y0), xmax - x1, y1 - y0, facecolor='none', hatch=hatch, edgecolor=color, lw=0, alpha=0.4))

    # Draw colored inner edges only
    lines = [
        Line2D([x0, x1], [y1, y1], color='C5', lw=1),  # top
        Line2D([x0, x1], [y0, y0], color='C5', lw=1),  # bottom
        Line2D([x0, x0], [y0, y1], color='C5', lw=1),  # left
        Line2D([x1, x1], [y0, y1], color='C5', lw=1),  # right
    ]
    for line in lines:
        ax.add_line(line)


def make_templates(
    tm: pd.DataFrame, 
    taus: np.ndarray, 
    N_template: int=0, 
    smearing: dict[str, object]=mu3e_smearing,
    rng: np.random.Generator|None=None, 
) -> dict:
    '''
    Generate reconstructed vertex distributions for a set of lifetime hypotheses.

    Returns a dictionary mapping each tested lifetime to its corresponding
    reconstructed longitudinal decay-vertex distribution.
    '''
    templates = {}

    for tau in taus:
        print(f"Making template for tau = {tau*1e12:.2f} ps")
        templates[tau] = _make_vz(tm, tau, N_template, smearing, rng=rng)

    return templates


def expected_precision(
    tm: pd.DataFrame, 
    templates: dict, 
    bins: np.ndarray, 
    tau_true: float=otmTau,
    N_data: int=10_000, 
    N_toys: int=100, 
    smearing: dict[str, object]=mu3e_smearing,
    rng: np.random.Generator|None=None, 
    verbose: int=0
) -> np.ndarray:
    '''
    Generate pseudo-experiments for estimating the statistical lifetime precision.

    For each pseudo-experiment, a reconstructed vertex sample containing N_data
    events is generated for tau_true and compared with the lifetime hypotheses.
    The returned distribution of reconstructed lifetimes can be used to determine
    the expected statistical uncertainty.
    '''
    rng = np.random.default_rng() if rng is None else rng

    fitted = []

    for _ in range(N_toys):
        data_vz = _make_vz(tm, tau_true, N_data, smearing, rng=rng)
        best_tau, _ = _compare_lifetimes(data_vz, templates, bins, verbose)
        fitted.append(best_tau)

    fitted = np.array(fitted)

    return fitted


    
#--------------------------------#
# Private functions              #
#--------------------------------#

    
def _tmDecay(
    tm: pd.DataFrame, 
    tau: float=otmTau, 
    rng: np.random.Generator|None=None
) -> tuple[pd.DataFrame, pd.DataFrame]:
    '''
    Simulate the two-body decay of true muonium into an electron--positron pair.

    Samples the decay vertex according to the specified lifetime, generates an
    isotropic two-body decay in the TM rest frame, and Lorentz-boosts the decay
    products into the laboratory frame.
    '''
    # create random-number generator if not provided
    rng = np.random.default_rng() if rng is None else rng
    
    # get the number of events
    nEvents = len(tm)

    # decay length in [mm]
    decL = 1e3 * tm.gamma * tm.beta * clight * tau
    
    # decay vertex magnitude in [mm]
    decV = rng.exponential(decL)
    
    # decay vertex coordinates in [mm]
    decVx = tm.x + (decV * (tm.px/tm.ptot))
    decVy = tm.y + (decV * (tm.py/tm.ptot))
    decVz = tm.z + (decV * (tm.pz/tm.ptot))

    # true muonium lab-frame momentum vectors in [MeV]
    p_tm = tm[['px', 'py', 'pz']].values

    # calculate kinematic parameters
    E_tm = np.sqrt(np.sum(p_tm**2, axis=1) + (2 * muonMass_MeV)**2)
    beta = p_tm / E_tm[:, np.newaxis]

    # energy and momentum of decay products in TM rest frame
    E_rest = (2 * muonMass_MeV) / 2
    p_rest_mag = np.sqrt(E_rest**2 - electronMass_MeV**2)

    # generate isotropic directions in TM rest frame
    phi = rng.uniform(0, 2*np.pi, size=nEvents)
    cos_theta = rng.uniform(-1, 1, size=nEvents)
    sin_theta = np.sqrt(1 - cos_theta**2)

    # electron momentum vectors in TM rest frame
    ele_px = p_rest_mag * sin_theta * np.cos(phi)
    ele_py = p_rest_mag * sin_theta * np.sin(phi)
    ele_pz = p_rest_mag * cos_theta
    ele_E = np.full(nEvents, E_rest)

    # positron momentum vectors in TM rest frame
    pos_px = -ele_px
    pos_py = -ele_py
    pos_pz = -ele_pz
    pos_E = ele_E

    # create rest frame 4-vectors
    ele_4vec = np.stack([ele_E, ele_px, ele_py, ele_pz], axis=1)  # shape: (nEvents, 4)
    pos_4vec = np.stack([pos_E, pos_px, pos_py, pos_pz], axis=1)

    # boost 4-vectors to lab frame
    ele_pLab = _lorentzBoost(ele_4vec, beta).T
    pos_pLab = _lorentzBoost(pos_4vec, beta).T

    # create output dataframes
    ele = pd.DataFrame({'x': decVx, 'y': decVy, 'z': decVz, 'Etot': ele_pLab[0], 'px': ele_pLab[1], 'py': ele_pLab[2], 'pz': ele_pLab[3], }, index=tm.index)
    pos = pd.DataFrame({'x': decVx, 'y': decVy, 'z': decVz, 'Etot': pos_pLab[0], 'px': pos_pLab[1], 'py': pos_pLab[2], 'pz': pos_pLab[3], }, index=tm.index)

    return ele, pos


def _lorentzBoost(
    p: np.ndarray, 
    beta: np.ndarray
) -> np.ndarray:
    '''
    Lorentz-boost an array of four-momentum vectors.

    Expects the four-momentum vectors with shape (N, 4), ordered as (E, px, py, pz)
    and the boost velocity vectors with shape (N, 3), expressed in units of c.
    '''
    beta_mag = np.linalg.norm(beta, axis=1)  # shape (N,)
    gamma = 1 / np.sqrt(1 - beta_mag**2)     # shape (N,)
    gamma2 = np.where(beta_mag > 0, (gamma - 1) / beta_mag**2, 0.0)  # shape (N,)

    # inner product p·β
    pDotBeta = np.sum(p[:,1:] * beta, axis=1)  # shape (N,)

    # energy component
    E_prime = gamma * (p[:,0] + pDotBeta)  # shape (N,)

    # spatial component
    term1 = p[:,1:]
    term2 = gamma2[:,np.newaxis] * pDotBeta[:,np.newaxis] * beta
    term3 = gamma[:,np.newaxis] * beta * p[:,0][:,np.newaxis]
    p_prime = term1 + term2 + term3  # shape (N, 3)

    return np.column_stack((E_prime, p_prime))  # shape (N, 4)


def _forward_intersection(
    particle: pd.DataFrame, 
    rDet: float
) -> np.ndarray:
    '''
    Calculate the forward intersection of straight particle trajectories with
    a cylindrical detector layer.

    The particle trajectory is propagated from its production point along its
    momentum direction until it reaches the detector radius rDet.
    '''

    # Normalize momentum vectors
    dx, dy, dz = particle.px.values/particle.ptot.values, particle.py.values/particle.ptot.values, particle.pz.values/particle.ptot.values

    # Coefficients for quadratic equation: a*t^2 + b*t + c = 0
    a = dx**2 + dy**2
    b = 2 * (particle.x * dx + particle.y * dy)
    c = particle.x**2 + particle.y**2 - rDet**2

    discriminant = b**2 - 4*a*c

    # Initialize output with NaNs
    intersection_points = np.full((len(particle),3), np.nan)

    # Only compute for valid discriminants
    valid = discriminant >= 0
    sqrt_disc = np.sqrt(discriminant[valid])
    t1 = (-b[valid] + sqrt_disc) / (2 * a[valid])
    t2 = (-b[valid] - sqrt_disc) / (2 * a[valid])

    # Choose the positive t (forward direction)
    t_forward = np.where(t1 > 0, t1, t2)

    # Compute intersection points
    intersection_points[valid, 0] = particle.x[valid] + t_forward * dx[valid]
    intersection_points[valid, 1] = particle.y[valid] + t_forward * dy[valid]
    intersection_points[valid, 2] = particle.z[valid] + t_forward * dz[valid]

    return intersection_points


def _smear_df_hits_and_mom(
    df: pd.DataFrame, 
    sigma_xyz_mm: tuple[float, float, float],
    sigma_theta_rad: float, 
    sigma_phi_rad: float,
    sigma_p_rel: float, 
    rng: np.random.Generator|None=None
) -> pd.DataFrame:
    '''
    Apply simplified detector smearing to particle hit positions and momenta.

    Hit coordinates are smeared with Gaussian spatial resolutions, while the
    momentum direction and magnitude are smeared according to angular and
    relative momentum resolutions. Default values from a Mu3e-like detector. 
    '''
    
    # create random-number generator if not provided
    rng = np.random.default_rng() if rng is None else rng
    
    out = df.copy()

    # smear hits
    sx, sy, sz = sigma_xyz_mm
    out["hitX"] = out["hitX"] + rng.normal(0.0, sx, size=len(out))
    out["hitY"] = out["hitY"] + rng.normal(0.0, sy, size=len(out))
    out["hitZ"] = out["hitZ"] + rng.normal(0.0, sz, size=len(out))

    # smear momentum by angles (+ optional magnitude)
    px = out["px"].to_numpy(float)
    py = out["py"].to_numpy(float)
    pz = out["pz"].to_numpy(float)
    p = np.sqrt(px*px + py*py + pz*pz)
    ok = p > EPS

    theta = np.zeros_like(p)
    phi = np.zeros_like(p)
    theta[ok] = np.arccos(np.clip(pz[ok] / p[ok], -1.0, 1.0))
    phi[ok] = np.arctan2(py[ok], px[ok])

    theta_s = theta + rng.normal(0.0, sigma_theta_rad, size=len(out))
    phi_s   = phi   + rng.normal(0.0, sigma_phi_rad,   size=len(out))

    p_s = p * (1.0 + rng.normal(0.0, sigma_p_rel, size=len(out)))
    p_s = np.clip(p_s, 0.0, None)

    out["px"] = p_s * np.sin(theta_s) * np.cos(phi_s)
    out["py"] = p_s * np.sin(theta_s) * np.sin(phi_s)
    out["pz"] = p_s * np.cos(theta_s)

    return out


def _reconstruct_vertex_from_lines(
    ele: pd.DataFrame, 
    pos: pd.DataFrame
) -> pd.DataFrame:
    '''
    Reconstruct the electron--positron decay vertex from straight-line tracks.

    Each track is defined by its detector hit position and momentum direction.
    The vertex is taken as the midpoint between the points of closest approach
    of the two tracks. The track distance of closest approach is also returned.
    '''
    
    rE = ele[["hitX", "hitY", "hitZ"]].to_numpy(float)
    rP = pos[["hitX", "hitY", "hitZ"]].to_numpy(float)

    pE = ele[["px", "py", "pz"]].to_numpy(float)
    pP = pos[["px", "py", "pz"]].to_numpy(float)

    pE_norm = np.linalg.norm(pE, axis=1)
    pP_norm = np.linalg.norm(pP, axis=1)

    ok_p = (pE_norm > EPS) & (pP_norm > EPS)

    uE = np.full_like(pE, np.nan)
    uP = np.full_like(pP, np.nan)
    uE[ok_p] = pE[ok_p] / pE_norm[ok_p, None]
    uP[ok_p] = pP[ok_p] / pP_norm[ok_p, None]

    w0 = rE - rP

    b = np.einsum("ij,ij->i", uE, uP)
    d = np.einsum("ij,ij->i", uE, w0)
    e = np.einsum("ij,ij->i", uP, w0)

    denom = 1.0 - b*b
    ok = ok_p & np.isfinite(denom) & (np.abs(denom) > 1e-10)

    tE = np.full(len(ele), np.nan)
    tP = np.full(len(ele), np.nan)

    tE[ok] = (b[ok] * e[ok] - d[ok]) / denom[ok]
    tP[ok] = (e[ok] - b[ok] * d[ok]) / denom[ok]

    xE = rE + tE[:, None] * uE
    xP = rP + tP[:, None] * uP

    v = 0.5 * (xE + xP)
    dca = np.linalg.norm(xE - xP, axis=1)

    return pd.DataFrame({
        "vx": v[:, 0],
        "vy": v[:, 1],
        "vz": v[:, 2],
        "dca": dca,
        "t_ele": tE,
        "t_pos": tP,
        "ok": ok,
    }, index=ele.index)


def _make_vz(
    tm: pd.DataFrame, 
    tau: float, 
    N: int=0, 
    smearing: dict[str, object]=mu3e_smearing,
    rng: np.random.Generator|None=None
) -> np.ndarray:
    '''
    Generate a reconstructed longitudinal decay-vertex distribution.

    True-muonium decays are generated for the specified lifetime, detector
    smearing is applied to the decay products, and the decay vertices are
    reconstructed from the resulting tracks.
    '''
    rng = np.random.default_rng() if rng is None else rng
    
    ele, pos = calcTM(tm, sample_size=N, tau=tau, rng=rng)

    ele_sm = _smear_df_hits_and_mom(ele, **smearing, rng=rng)
    pos_sm = _smear_df_hits_and_mom(pos, **smearing, rng=rng)

    vtx = _reconstruct_vertex_from_lines(ele_sm, pos_sm)

    return vtx.vz.to_numpy()


def _poisson_nll(
    data_vz: np.ndarray, 
    template_vz: np.ndarray, 
    bins: np.ndarray
) -> float:
    '''
    Calculate the binned Poisson likelihood statistic between two vertex distributions.

    The Monte Carlo distribution is normalized to the number of data events and
    compared bin-by-bin using the Poisson likelihood-ratio statistic.
    '''
    n_data, _ = np.histogram(data_vz, bins=bins)
    n_temp, _ = np.histogram(template_vz, bins=bins)

    mu = n_temp / n_temp.sum() * n_data.sum()
    mu = np.clip(mu, EPS, None)

    mask = n_data > 0

    return (
        2 * np.sum(mu - n_data)
        + 2 * np.sum(n_data[mask] * np.log(n_data[mask] / mu[mask]))
    )


def _compare_lifetimes(
    data_vz: np.ndarray, 
    templates: dict, 
    bins: np.ndarray, 
    verbose: int=0
) -> tuple[float, np.ndarray]:
    '''
    Determine the lifetime best describing a reconstructed vertex distribution.

    The data distribution is compared to Monte Carlo predictions for different
    lifetime hypotheses using a binned Poisson likelihood. A quadratic fit to
    the likelihood scan is used to determine the best lifetime continuously
    between the tested hypotheses.
    '''
    taus = np.asarray(list(templates.keys()))
    
    nll = np.array([
        _poisson_nll(data_vz, templates[tau], bins)
        for tau in taus
    ])

    delta = nll - np.min(nll)

    popt = np.polyfit(1e12*taus, delta, 2)
    best_tau = -popt[1]/2/popt[0] / 1e12

    if verbose>1:
        fig, ax = plt.subplots()
        ax.plot(1e12*taus, delta, '.')
        ax.plot(1e12*taus, np.polyval(popt, 1e12*taus), 'C1') 
        ax.axhline(1.0, linestyle='--', label=r'$\Delta(-2\ln L)=1$')
        ax.axvline(1e12*best_tau, c='C1', linestyle="--", label=fr'best $\tau={1e12*best_tau:.2f}$ ps')
        ax.set(xlabel=r'assumed lifetime $\tau$ [ps]', ylabel=r'$\Delta(-2\ln L)$')
        ax.legend()
        plt.show()

    return best_tau, nll

