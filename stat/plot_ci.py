import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import matplotlib.ticker as ticker

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

def plot_comparison(mean1, lb1, ub1, mean2, lb2, ub2, delta, save_path, show, dpi=300,
                    xlabel=r"Number of Iterations ($t$)",
                    ylabel=r"$\hat{R}_{bal}$",
                    title=r"Comparison of confidence intervals"):

    c1_line = '#4B0082'
    c1_fill  = '#E6E6FA'
    c2_line = '#006400'
    c2_fill  = '#CCFF00'

    fig, ax = plt.subplots()

    x_axis = np.arange(0, len(lb1))
    ax.plot(x_axis, mean1, color=c1_line, label=r"Naive MC", linestyle='--')
    ax.fill_between(x_axis, lb1, ub1, color=c1_fill, alpha=0.5, label=r"Hoeffding, $" + str(100.0*(1 - delta)) + r"\%$ CI", edgecolor=None)

    ax.plot(x_axis, mean2, color=c2_line, label=r"Ours", linestyle='-')
    ax.fill_between(x_axis, lb2, ub2, color=c2_fill, alpha=0.5, label=r"Delta method, $" + str(100.0*(1-delta)) + r"\%$ CI", edgecolor=None)

    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)

    instance_name = save_path.split("/")[-1].split(".")[0]
    instance_name = instance_name[0].upper() + instance_name[1:].lower()
    ax.set_title(instance_name)

    ax.yaxis.set_major_formatter(ticker.ScalarFormatter(useMathText=True))

    lloc = r'upper right' if ub1[-1] < 0.5 else r'lower right'
    ax.legend(loc=lloc, frameon=True, fancybox=False, edgecolor='k', fontsize=9)

    plt.tight_layout()

    if save_path:
        fig.savefig(save_path, dpi=dpi, bbox_inches="tight")

    if show:
        plt.show()
    else:
        plt.close(fig)
