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
    files = ("matmul256_base",
             "matmul256_v_vec8",
             "matmul256_v_vec16",
             "matmul256_v_vec32",

             "conv256_base",
             "conv256_v_vec8",
             "conv256_v_vec16",
             "conv256_v_vec32",
             
             "fc1024_base",
             "fc1024_v_vec8",
             "fc1024_v_vec16",
             "fc1024_v_vec32",
             
             "maxpooling256_base",
             "maxpooling256_v_vec8",
             "maxpooling256_v_vec16",
             "maxpooling256_v_vec32",
             
             "mlenet5_base",
             "mlenet5_v_vec8",
             "mlenet5_v_vec16",
             "mlenet5_v_vec32"
             )

    long_names = ("MATRIX MULTIPLICATION - Scalar",
                  "MATRIX MULTIPLICATION - Vec-8",
                  "MATRIX MULTIPLICATION - Vec-16",
                  "MATRIX MULTIPLICATION - Vec-32",

                  "CONVOLUTION - Scalar",
                  "CONVOLUTION - Vec-8",
                  "CONVOLUTION - Vec-16",
                  "CONVOLUTION - Vec-32",
                  
                  "FULLY CONNECTED - Scalar",
                  "FULLY CONNECTED - Vec-8",
                  "FULLY CONNECTED - Vec-16",
                  "FULLY CONNECTED - Vec-32",
                  
                  "MAXPOOLING - Scalar",
                  "MAXPOOLING - Vec-8",
                  "MAXPOOLING - Vec-16",
                  "MAXPOOLING - Vec-32",
                  
                  "Lenet5 - Scalar",
                  "Lenet5 - Vec-8",
                  "Lenet5 - Vec-16",
                  "Lenet5 - Vec-32"
                  )

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
