import numpy as np
import pandas as pd
from cycler import cycler

from scipy.constants import physical_constants
import matplotlib.patches as patches
from matplotlib.lines import Line2D


# muon mass in [MeV]
muonMass_MeV = physical_constants['muon mass energy equivalent in MeV'][0]

# electron mass in [MeV]
electronMass_MeV = physical_constants['electron mass energy equivalent in MeV'][0]

# plot settings for presentations, posters, regular
pres = {'figure.figsize': (6,4),
        'axes.titlecolor':'white',
        'axes.edgecolor':'white', 
        'font.size': 12,
        'xtick.color':'white', 
        'ytick.color':'white', 
        'figure.facecolor':'none', 
        'axes.labelcolor':'white',
        'axes.facecolor':'none', 
        'legend.fontsize': 10,
        'legend.facecolor': (0.2,0.2,0.2),
        'legend.framealpha': 1,
        'legend.labelcolor':'white',
        'legend.labelspacing': 0.1,
        'figure.dpi':200,
        'axes.prop_cycle':cycler(color=['deepskyblue', 'orange', 'yellowgreen', 'tomato', 'orchid', 'w']),
        }

post = {'figure.figsize': (6,4),
        'font.size': 12,
        'legend.fontsize': 10,
        'legend.framealpha':1,
        'legend.labelspacing': 0.1,
        'figure.dpi':300,
        'axes.prop_cycle':cycler(color=['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#000000']),
       }

regu = {'figure.figsize': (6,4),
        'font.size': 12,
        'legend.fontsize': 10,
        'legend.framealpha':1,
        'legend.labelspacing': 0.1,
        'figure.dpi':100,
        'axes.prop_cycle':cycler(color=['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#000000']),
       }

def bin_range(edges, low=None, high=None, inclusive_high=False):
    """
    Returns a slice selecting bins whose interval overlaps [low, high)
    (or [low, high] if inclusive_high=True).
    """
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

def lorentzBoost(p, beta):
    '''
    p     : shape (N, 4) — four-momentum vectors
    beta  : shape (N, 3) — boost vectors for each event
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

def tmDecay(tm):
    
    # get the number of events
    nEvents = len(tm)

    # true muonium lab-frame momentum vectors in [MeV]
    p_tm = tm[['px', 'py', 'pz']].values

    # calculate kinematic parameters
    E_tm = np.sqrt(np.sum(p_tm**2, axis=1) + (2 * muonMass_MeV)**2)
    beta = p_tm / E_tm[:, np.newaxis]

    # energy and momentum of decay products in TM rest frame
    E_rest = (2 * muonMass_MeV) / 2
    p_rest_mag = np.sqrt(E_rest**2 - electronMass_MeV**2)

    # generate isotropic directions in TM rest frame
    phi = np.random.uniform(0, 2*np.pi, size=nEvents)
    cos_theta = np.random.uniform(-1, 1, size=nEvents)
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
    ele_pLab = lorentzBoost(ele_4vec, beta)
    pos_pLab = lorentzBoost(pos_4vec, beta)

    return pd.DataFrame(ele_pLab, columns=['Etot', 'px', 'py', 'pz']), pd.DataFrame(pos_pLab, columns=['Etot', 'px', 'py', 'pz'])

def forward_intersection(tm, ele, rDet):
    """
    Compute the forward intersection point of a particle trajectory with a circle of given radius in the xy-plane.

    Parameters:
    - tm: TM DataFrame, needs the columns decVx, decVy, and decVz
    - momentum_vectors: array of shape (N, 3), each row is (px, py, pz)
    - rDet: scalar radial distance in xy-plane

    Returns:
    - intersection_points: array of shape (N, 3), each row is (x, y, z) of the forward intersection point
    """
    
    # Normalize momentum vectors
    dx, dy, dz = ele.px.values/ele.ptot.values, ele.py.values/ele.ptot.values, ele.pz.values/ele.ptot.values

    # Coefficients for quadratic equation: a*t^2 + b*t + c = 0
    a = dx**2 + dy**2
    b = 2 * (tm.decVx * dx + tm.decVy * dy)
    c = tm.decVx**2 + tm.decVy**2 - rDet**2

    discriminant = b**2 - 4*a*c

    # Initialize output with NaNs
    intersection_points = np.full((len(ele),3), np.nan)

    # Only compute for valid discriminants
    valid = discriminant >= 0
    sqrt_disc = np.sqrt(discriminant[valid])
    t1 = (-b[valid] + sqrt_disc) / (2 * a[valid])
    t2 = (-b[valid] - sqrt_disc) / (2 * a[valid])

    # Choose the positive t (forward direction)
    t_forward = np.where(t1 > 0, t1, t2)

    # Compute intersection points
    intersection_points[valid, 0] = tm.decVx[valid] + t_forward * dx[valid]
    intersection_points[valid, 1] = tm.decVy[valid] + t_forward * dy[valid]
    intersection_points[valid, 2] = tm.decVz[valid] + t_forward * dz[valid]

    return intersection_points

def combinatorialAnalysis (ele, pos, which='alpha'):

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

    # calculate distance of closest approach
    elif which=='dca':

        # reference positions: hit coordinates
        pos_x = pos["x"].to_numpy()[:, np.newaxis]
        pos_y = pos["y"].to_numpy()[:, np.newaxis]
        pos_z = pos["z"].to_numpy()[:, np.newaxis]
    
        ele_x = ele["x"].to_numpy()[np.newaxis, :]
        ele_y = ele["y"].to_numpy()[np.newaxis, :]
        ele_z = ele["z"].to_numpy()[np.newaxis, :]

        # unit direction vectors from momenta
        ux_pos = pos_px / pos_ptot
        uy_pos = pos_py / pos_ptot
        uz_pos = pos_pz / pos_ptot
    
        ux_ele = ele_px / ele_ptot
        uy_ele = ele_py / ele_ptot
        uz_ele = ele_pz / ele_ptot
    
        # cross product: u_pos x u_ele
        cx = uy_pos * uz_ele - uz_pos * uy_ele
        cy = uz_pos * ux_ele - ux_pos * uz_ele
        cz = ux_pos * uy_ele - uy_pos * ux_ele
    
        cross_norm = np.sqrt(cx**2 + cy**2 + cz**2)
    
        # displacement between hit points
        dx = ele_x - pos_x
        dy = ele_y - pos_y
        dz = ele_z - pos_z
    
        numerator = np.abs(dx * cx + dy * cy + dz * cz)
    
        eps = 1e-12
        dca = np.where(cross_norm > eps, numerator / cross_norm, np.nan)
    
        return dca

    else:
        print(f'method "{which}" not implemented')

def add_hatched_exclusion(ax, x0, x1, y0, y1,
                          hatch='///', color='C5',
                          zorder=0, label=None):
    """
    Hatches the entire plotting area except for a rectangular hole.

    Parameters
    ----------
    ax : matplotlib.axes.Axes
        The axes to draw on.
    x0, x1, y0, y1 : float
        Coordinates of the hole (data coordinates).
    hatch : str, optional
        Hatch pattern (e.g. '/', '\\', 'x', 'o', etc.).
    color : str, optional
        Hatch line color.
    zorder : int, optional
        Drawing order.
    label : str, optional
        label of the patch in the figure
    """

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
