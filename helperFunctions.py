import numpy as np
from cycler import cycler

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