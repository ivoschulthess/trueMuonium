import numpy as np
from scipy.constants import physical_constants
from cycler import cycler



# muon mass in [MeV]
muonMass_MeV = physical_constants['muon mass energy equivalent in MeV'][0]

# electron mass in [MeV]
electronMass_MeV = physical_constants['electron mass energy equivalent in MeV'][0]

# speed of light in vacuum in [m/s]
clight = physical_constants['speed of light in vacuum'][0]

# proper lifetime of oTM in [s]
otmTau = 1.81e-12



# numerical tolerance
EPS = 1e-12

# cross-section bias for µ+µ-
muonPairBias = 1e6



# number of simulated primary photons
NSimBkg = 4e13

# initial photon beam energy in [MeV]
photonEnergy = 300

# Lorentz factor of the TM atom
lorentzFactor = photonEnergy/(2*muonMass_MeV)

# relativistic velocity of the TM atom
relVel = np.sqrt(1 - 1/lorentzFactor**2)

# dissociation length of Pb in [mm]
dissociationLength = 0.00346823 # in [mm]

# target thickness in [mm]
targetThickness = 2 * dissociationLength

# inner radius of the detector in [mm]
detectorRadius = 150

# ratio of cross sections for µ+µ- / TM
boundStateSuppression = 61310060



# minimal energy in [MeV]
E_th_min = lorentzFactor * (muonMass_MeV - relVel*np.sqrt(muonMass_MeV**2 - electronMass_MeV**2))
E_th_max = lorentzFactor * (muonMass_MeV + relVel*np.sqrt(muonMass_MeV**2 - electronMass_MeV**2))
print(f'E in [{E_th_min:.1f}, {E_th_max:.1f}] MeV')

# maximal longitudinal position in [mm]
z_th_min = -500
z_th_max = 1000
print(f'z in [{z_th_min:.0f}, {z_th_max:.0f}] mm')



# Mu3e-like detector smearing
mu3e_smearing = {
    "sigma_xyz_mm": (0.023, 0.023, 0.023),
    "sigma_theta_rad": 0.004,
    "sigma_phi_rad": 0.004,
    "sigma_p_rel": 0.02,
}



# plot settings
regPlot = {'figure.figsize': (6,4),
        'font.size': 12,
        'legend.fontsize': 10,
        'legend.framealpha':1,
        'legend.labelspacing': 0.1,
        'figure.dpi':100,
        'axes.prop_cycle':cycler(color=['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#000000']),
       }