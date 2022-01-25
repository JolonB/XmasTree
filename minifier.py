import csv
import struct

VERSION = "1.0"

filename = "points.csv"
out_filename = "points.xmas"

def minify():
    with open(filename, "r") as f, open(out_filename, "wb") as out_f:
        reader = csv.reader(f)

        header = next(reader)

        if (len(header) - 1) % 3 != 0:
            raise ValueError("Invalid header length. Must contain LED triplets.")

        led_count = (len(header) - 1) // 3

        out_f.write(b"---\n")
        out_f.write("Version:{}\n".format(VERSION).encode())
        out_f.write("NumLED:{}\n".format(led_count).encode())
        out_f.write("Framerate:{}\n".format(5).encode())
        out_f.write(b"---\n")

        struct_string = "B"*led_count*3

        for row in reader:
            # Stop if row is blank
            if not row:
                break

            led_id = row[0] # unused

            # led_data = struct.pack(struct_string, *[int(x) for x in row[1:]])
            for data in row[1:]:
                led_data = struct.pack('B', int(data))
                out_f.write(led_data)

            # out_f.write("\n")

def deminify():
    with open(out_filename, "r") as f, open("deminified.csv", "w") as out_f:
        header = f.readline()
        led_count = int(header.split(":")[1])
        header = f.readline()
        framerate = int(header.split(":")[1])
        f.readline() # this should be "---"

        output_header = ["FRAME_ID"]
        for i in range(led_count):
            output_header.extend(["R_{}".format(i), "G_{}".format(i), "B_{}".format(i)])
        out_f.write(",".join(output_header))

        frame_id = 0
        out_f.write("\n{},".format(frame_id))
        while True:
            # breakpoint()
            c = f.read(1)
            if not c:
                break
            # Check if \n
            if c == "\n":
                frame_id += 1
                out_f.write("\n{},".format(frame_id))
                continue
            num = ord(c)
            out_f.write(str(num))
            out_f.write(",")


if __name__ == "__main__":
    minify()
