import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import sys
import os

# -------------------------------------------------------
# Pouzitie: python boxplot.py ExperimentResults_HHMMSS.csv
# -------------------------------------------------------

if len(sys.argv) < 2:
    print("Pouzitie: python boxplot.py <ExperimentResults_HHMMSS.csv>")
    sys.exit(1)

filename = sys.argv[1]
if not os.path.exists(filename):
    print(f"Subor nenajdeny: {filename}")
    sys.exit(1)

# Nacitanie CSV (separator = ; )
df = pd.read_csv(filename, sep=';', encoding='cp1252')
df.columns = df.columns.str.strip()

# Premenujeme stlpce na jednotne nazvy
df.rename(columns={
    df.columns[0]: "Ruka",
    df.columns[1]: "Gesto",
    df.columns[2]: "ReakcnyCas"
}, inplace=True)

df["ReakcnyCas"] = pd.to_numeric(df["ReakcnyCas"], errors='coerce')
df.dropna(subset=["ReakcnyCas"], inplace=True)

# Poradie gest - zostupne podla priemerneho reakčného času
order = (df.groupby("Gesto")["ReakcnyCas"]
           .mean()
           .sort_values(ascending=False)
           .index.tolist())

# Farby podla ruky
hand_map = df.groupby("Gesto")["Ruka"].first().to_dict()
color_map = {"LEFT": "#1E90FF", "RIGHT": "#E53935"}

fig, ax = plt.subplots(figsize=(12, max(6, len(order) * 0.45)))

data_per_gesture = [df[df["Gesto"] == g]["ReakcnyCas"].values for g in order]
colors = [color_map.get(hand_map.get(g, "RIGHT"), "#1565C0") for g in order]

bp = ax.boxplot(
    data_per_gesture,
    vert=False,
    patch_artist=True,
    positions=range(len(order)),
    widths=0.6,
    medianprops=dict(color="white", linewidth=2),
    whiskerprops=dict(color="#555555"),
    capprops=dict(color="#555555"),
    flierprops=dict(marker='o', color='#aaaaaa', markersize=4)
)

for patch, color in zip(bp['boxes'], colors):
    patch.set_facecolor(color)
    patch.set_alpha(0.85)

ax.set_yticks(range(len(order)))
ax.set_yticklabels(order, fontsize=9)
ax.set_xlabel("Reakčný čas [ms]", fontsize=11)
ax.set_title("Reakčný čas podľa gesta", fontsize=13, fontweight='bold')
ax.grid(axis='x', linestyle='--', alpha=0.5)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)

left_patch  = mpatches.Patch(color="#1E90FF", alpha=0.85, label="Ľavá ruka")
right_patch = mpatches.Patch(color="#E53935", alpha=0.85, label="Pravá ruka")
ax.legend(handles=[left_patch, right_patch], loc="upper right", fontsize=10)

plt.tight_layout()

outfile = filename.replace(".csv", "_boxplot.png")
plt.savefig(outfile, dpi=150, bbox_inches='tight')
print(f"Graf uložený: {outfile}")
