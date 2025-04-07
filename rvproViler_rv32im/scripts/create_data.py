from collections import defaultdict
import csv
from matplotlib import pyplot as plt


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


def main():
    files = ("matmul256_base",)

    long_names = ("MATRIX MULTIPLICATION - Scalar",)

    old_fnt = plt.rcParams['font.size']
    plt.rcParams['font.size'] = 12

    for file, long_name in zip(files, long_names):
        total, name2count = create_mapping(f"../out/{file}")
        names = list(name2count.keys())
        counts = list(name2count.values())

        # Write data to CSV file
        csv_filename = f"../csv/{file}.csv"
        with open(csv_filename, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['Instruction', 'Instruction Call Frequency'])
            for name, count in zip(names, counts):
                writer.writerow([name, count])
            writer.writerow([])  # Empty row for spacing
            writer.writerow(['Total Instruction Calls', total])  # Write total instruction calls

        bar_width = 0.6
        fig, ax = plt.subplots(layout='constrained', figsize=(5, 5))

        rects = ax.bar(range(len(counts)), counts, width=bar_width)
        ax.bar_label(rects, padding=2, rotation="vertical")
        ax.set_xticks(range(len(counts)), names, rotation="vertical")
        ax.set_yscale('log')
        ax.set_ylim(min(counts) * .99, max(counts) * 10000)
        plt.title(f"{long_name}\nTotal Instruction Calls: {total}")
        plt.xlabel("Instruction")
        plt.ylabel("Instruction Call Frequency")
        plt.savefig(f"../plots/{file}.png")

    plt.rcParams['font.size'] = old_fnt


if __name__ == "__main__":
    main()
