from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt


def create_mapping(file_name):
    addr2name = {}
    with open(f"{file_name}.dump", "r") as f:
        for line in f.readlines():
            items = line.split()
            if len(items) >= 3:
                addr = items[0][:-1]
                label = items[2]
                addr2name[addr] = label

    addr2count = {}
    with open(f"{file_name}.his", "r") as f:
        for line in f.readlines()[1:]:
            items = line.split()
            if len(items) >= 2:
                addr = items[0]
                count = items[1]
                addr2count[addr] = int(count)
    name2count = defaultdict(int)
    for addr, count in addr2count.items():
        name2count[addr2name[addr]] += count
    total_count = sum(int(count) for _, count in name2count.items())
    return total_count, {k: v for k, v in sorted(name2count.items(), key=lambda item: int(item[1]))}


def make_conf2ins():
    conf2ins = {"base": [], "simd16": [], "simd8": []}
    for conf in ["base", "p_simd16", "p_simd8"]:
        for file in ["conv256", "fc1024", "matmul256", "maxpooling256", "mlenet5"]:
            total, _ = create_mapping(f"../out/{file}_{conf}")
            conf2ins[conf.removeprefix("p_")] += [total]

    for conf in ["p_simd16", "p_simd8"]:
        for file in range(5):
            conf2ins[conf.removeprefix("p_")][file] = conf2ins["base"][file] / conf2ins[conf.removeprefix("p_")][file]

    for file in range(5):
        conf2ins["base"][file] = 1

    return conf2ins


def main():
    bms = ["Conv.\n256x256x1\n4x4x4 Kernel",
           "Fully Conn.\n1024->1024",
           "Mat. Mult.\n256x256",
           "Max-Pool.\n256x8",
           "LeNet-5"]

    conf2ins = make_conf2ins()

    old_fnt = plt.rcParams['font.size']
    plt.rcParams['font.size'] = 12

    x = np.array([2 * i for i in np.arange(len(bms))])
    width = 0.5
    multiplier = 0
    max_val = 0
    fig, ax = plt.subplots(layout='constrained', figsize=(7, 5))
    for attribute, measurement in conf2ins.items():
        offset = width * multiplier
        measurement_scaled = [round(i, 2) for i in measurement]
        max_val = max(max_val, max(measurement_scaled))
        rects = ax.bar(x + offset, measurement_scaled, width=width, label=str(attribute), )
        ax.bar_label(rects, padding=2, rotation="vertical")
        multiplier += 1

    ax.set_xlabel('Benchmark Type')
    ax.set_ylabel('Speedup\n(Higher is better)')
    ax.set_xticks(x + width, bms, rotation="vertical")
    ax.legend(ncol=3)
    ax.set_title(f"Evaluation Summary")
    ax.set_ylim(0, max_val * 1.4)
    # plt.show()
    plt.savefig(f'../plots/pre-eval-summary.png')

    plt.rcParams['font.size'] = old_fnt


if __name__ == '__main__':
    main()
