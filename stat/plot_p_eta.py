import matplotlib
import matplotlib.pyplot as plt
import numpy as np

matplotlib.rcParams.update({
    "text.usetex": True,
    "text.latex.preamble": r"""
        \usepackage{amsmath}
        \usepackage{amssymb}
    """,
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
    "mathtext.fontset": "cm",
    "axes.labelsize": 12,
    "font.size": 11,
    "legend.fontsize": 10,
    "xtick.labelsize": 10,
    "ytick.labelsize": 10,
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

def plot_rows(instance_name, data, p_mul, save_path=None, show=False, dpi=300):
    fig, ax = plt.subplots(figsize=(6.5, 4.0))

    for i, row in enumerate(data):
        x = np.arange(1, len(row) + 1)
        ax.plot(
            x,
            row,
            lw=2.0,
            marker="o",
            color=ROW_COLORS[i % len(ROW_COLORS)],
        )

    if p_mul is not None:
        p_mul = np.asarray(p_mul)
        assert len(p_mul) == len(data[0])
        ax.set_xticks(np.arange(1, len(p_mul) + 1))
        ax.set_xticklabels([
            rf"${v:.2f}$" for v in p_mul
        ])

    ax.set_title(instance_name)
    ax.set_xlabel(r"Probability factor")
    ax.set_ylabel(r"$\hat{R}_{bal}$")
    ax.grid(True, alpha=0.3)

    plt.tight_layout()

    if save_path:
        fig.savefig(save_path, dpi=dpi, bbox_inches="tight")

    if show:
        plt.show()
    else:
        plt.close(fig)