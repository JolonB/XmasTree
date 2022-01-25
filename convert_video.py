
import argparse

import cv2
import progressbar
import numpy as np

# Parse arguments
parser = argparse.ArgumentParser(description="Map a video file to Xmas tree coordinates.")
parser.add_argument("video", help="The video file to use.", type=str)
parser.add_argument("tree_coords", help="The tree coordinates file to use.", type=str)
parser.add_argument("-f", "--fps", help="The desired framerate. Defaults to 10", default=10, type=float)
parser.add_argument("-o", "--output", help="The output file. Defaults to points.csv", default="points.csv", type=str)
parser.add_argument("--slow", help="Use a slower algorithm. This can be used if there are errors during processing, although an error doesn't necessarily mean the output is incorrect", action="store_true")


def convert(video_file, tree_coords, desired_fps, output_file, slow=False):
    coords = []
    # Read coordinates file
    with open(tree_coords, "r", encoding="utf-8-sig") as file:
        for line in file:
            x, _, z = line.split(",")
            # Since we are working with 2d images, we only keep x (horiz)
            # and z (vert) coordinates
            coords.append((float(x), float(z)))

    # Get the number of LEDs
    num_leds = len(coords)

    # Get min and max x coord
    min_x = min(coords, key=lambda x: x[0])[0]
    max_x = max(coords, key=lambda x: x[0])[0]
    # Get min and max z coord
    min_z = min(coords, key=lambda x: x[1])[1]
    max_z = max(coords, key=lambda x: x[1])[1]

    # Get tree height and width
    tree_height = max_z - min_z
    tree_width = max_x - min_x

    if tree_height <= 0 or tree_width <= 0:
        raise ValueError("Invalid tree dimensions. Height and width must be"
                            " greater than 0.")

    # Read video file
    cap = cv2.VideoCapture(video_file)
    # Get the frame count and framerate
    frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    frame_rate = float(cap.get(cv2.CAP_PROP_FPS))
    # Get the frame size
    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)) - 1
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)) - 1

    if frame_width == 0 or frame_height == 0:
        raise Exception("Frame size is 0")
    
    # Get height and width scaling factor
    scale_height = frame_height / tree_height
    scale_width = frame_width / tree_width

    # Get fps scaling factor (cannot be greater than 1)
    if desired_fps <= 0:
        raise ValueError("Desired frame rate must be greater than 0")
    every_nth_frame = max(1., frame_rate / desired_fps)

    # Only use the minimum scale factor, otherwise the tree will be too big
    # which won't maximise the effective video frame size
    scale = min(scale_height, scale_width)

    # Scale and shift the coordinates to fit the video frame
    scaled_coords = []
    for x, z in coords:
        # This assumes that x is -1 to 1 and z is 0 to anything
        scaled_coords.append((int(x * scale + frame_width / 2), int(frame_height - (z - min_z) * scale)))

    scaled_coords = np.array(scaled_coords, dtype=np.int16)
    scaled_coords = [*scaled_coords.T]

    exists, frame = cap.read()
    with open(output_file, "w", newline='') as file:

        header = ["FRAME_ID"]
        for i in range(num_leds):
            header.extend("R_{0} G_{0} B_{0}".format(i).split())
        file.write(",".join(header) + "\n")

        frame_id = 0
        for frame_no in progressbar.progressbar(np.arange(0, frame_count, every_nth_frame)):
            frame_no = int(frame_no)

            if not slow:
                # Jump to the desired frame if slow isn't set
                cap.set(cv2.CAP_PROP_POS_FRAMES, frame_no)
            else:
                # Iterate until we reach the correct frame if slow is set
                frame_tmp = cap.get(cv2.CAP_PROP_POS_FRAMES)
                while frame_tmp < frame_no:
                    exists, frame = cap.read()
                    frame_tmp = cap.get(cv2.CAP_PROP_POS_FRAMES)

            exists, frame = cap.read()

            if not exists:
                raise Exception("Frame does not exist. There must be a problem with the video file.")

            # Process the frame
            out = frame[scaled_coords[1], scaled_coords[0]]
            # Convert from BGR to RGB
            out = out[:, ::-1] # could use np.flip, but this is faster

            # Add the frame ID to the start of the row
            row = [frame_id]
            # Add the frame data to the row
            row.extend(out.flatten())
            # Write the row to the file
            file.write(",".join(map(str, row)) + "\n")

            frame_id += 1

if __name__ == "__main__":
    args = parser.parse_args()

    video_file = args.video
    tree_coords = args.tree_coords
    desired_fps = args.fps
    output_file = args.output
    slow = args.slow

    convert(video_file, tree_coords, desired_fps, output_file, slow=slow)
