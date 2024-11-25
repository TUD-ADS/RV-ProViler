import tempfile
import os

def find_beg_end(in_file: str) -> (int, int):
    with open(in_file, "r") as in_f:
        state = 0
        hex1: str
        hex2: str
        lines = in_f.readlines()
        for idx, line in enumerate(lines):
            if state == 1:
                if "puts" in line:
                    hex2 = lines[idx-1].split(":")[0].strip()
                    break

            if state == 0:
                if "puts" in line:
                    hex1 = lines[idx+1].split(":")[0].strip()
                    state = 1

        return int(hex1, 16), int(hex2, 16)

def filter_his(file: str, beg: int, end: int):
    with open(file, "r+") as f:
        f.seek(0)

        with tempfile.TemporaryFile("a+", encoding="utf8") as tmp_f:
            size: int
            state = 0
            for line in f:
                if state == 1:
                    addr = int(line.split(" ")[0], 16)
                    if beg <= addr <= end:
                        tmp_f.writelines([line])
                    else:
                        size -= 1

                if state == 0:
                    size = int(line.split(":")[1], 10)
                    state = 1

            tmp_f.seek(0)
            f.seek(0)
            f.truncate()
            f.writelines([f"PC Histogram size:{size}\n"])
            f.writelines(tmp_f.readlines())


if __name__ == "__main__":
    dir = os.path.dirname(os.path.realpath(__file__))
    dir += "/../out/"

    bm_list = ["matmul256", "fc1024", "maxpooling256", "conv256", "mlenet5"]
    conf_list = ["base", "p_simd8", "p_simd16"]

    for bm in bm_list:
        for conf in conf_list:
            file_name = f"{bm}_{conf}"
            beg, end = find_beg_end(f"{dir}{file_name}.dump")
            filter_his(f"{dir}{file_name}.his", beg, end)

