# legend_only.py

import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

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

def plot_eta_legend(values, save_path, ncol=1):

    handles = []
    labels = []

    for i, v in enumerate(values):
        color = ROW_COLORS[i % len(ROW_COLORS)]

        handles.append(
            Line2D(
                [0], [0],
                color=color,
                lw=2.5
            )
        )

        labels.append(rf"${100*v}\%$")

    fig = plt.figure(figsize=(2.0, 0.5 * len(values)))
    fig.legend(
        handles,
        labels,
        loc="center",
        frameon=False,
        ncol=ncol,
        title=r"Value of $\eta$",
        handlelength=2.8,
        labelspacing=0.8,
        columnspacing=1.4,
    )

    plt.axis("off")

    fig.savefig(save_path, bbox_inches="tight")
    plt.close(fig)
