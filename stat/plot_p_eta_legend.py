import matplotlib
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from matplotlib.patches import Patch
from matplotlib.patches import Rectangle
from matplotlib.offsetbox import AnchoredOffsetbox, HPacker, TextArea, DrawingArea


matplotlib.rcParams.update({
    "text.usetex": True,
    "text.latex.preamble": r"""
        \usepackage{amsmath}
        \usepackage{amssymb}
    """,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
    "mathtext.fontset": "cm",
    "axes.labelsize": 20,
    "font.size": 20,
    "legend.fontsize": 20,
    "xtick.labelsize": 20,
    "ytick.labelsize": 20,
    "lines.linewidth": 1.8,
    "lines.markersize": 4.5,
    "axes.grid": True,
    "grid.alpha": 0.3,
    "grid.linestyle": "--",
})

ROW_COLORS = [
    "#2f855a",
    "#b7791f",
    "#2b6cb0",
    "#805ad5",
    "#718096",
    "#c05621",
    "#285e61",
    "#744210",
]


MARKERS = ['x', '^', 'v', 'D', 'o', 's', '*', 'h']

def plot_eta_legend(values, save_path):
    items = []

    items.append(
        TextArea(r"Values of $\eta, \; \%:$\qquad", textprops=dict(size=11))
    )

    for i, v in enumerate(values):
        da = DrawingArea(8, 14, 0, 0)

        marker_artist = Line2D(
            [7], [7],                 # centered in the drawing area
            marker=MARKERS[i],
            markersize=7,
            markerfacecolor=ROW_COLORS[i],
            markeredgecolor=ROW_COLORS[i],
            linestyle='None'
        )

        da.add_artist(marker_artist)
        items.append(da)

        items.append(
            TextArea(r"${}\;$".format(100.0 * v), textprops=dict(size=11))
        )

    packed = HPacker(children=items, align="center", pad=0.5, sep=12)

    box = AnchoredOffsetbox(
        loc="center",
        child=packed,
        frameon=True,
        borderpad=0.1
    )

    fig, ax = plt.subplots(figsize=(len(values) * 0.9, 0.85))
    ax.axis("off")
    ax.add_artist(box)

    # Style the frame
    frame = box.patch
    frame.set_edgecolor("0.4")
    frame.set_linewidth(0.1)
    frame.set_facecolor("white")

    plt.savefig(save_path, bbox_inches="tight", pad_inches=0.00)
    plt.close(fig)