
import argparse

import cv2
import progressbar

DOWNSCALE_DIM = 144

# Parse arguments
parser = argparse.ArgumentParser(description="Blur and compress a video and save it as an mp4.\nThis may give you better outputs than just using the original video because multiple pixels are blurred into one value.")
parser.add_argument("video", help="The video file to use.", type=str)
parser.add_argument("-o", "--output", help="The output file. Defaults to %(default)s", default="output.mp4", type=str)
parser.add_argument("-d", "--downscale", help="Downscale the video so that the smallest dimesion is {} pixels. Defaults to %(default)s".format(DOWNSCALE_DIM), default=False, action="store_true")
parser.add_argument("-i", "--intensity", help="The intensity of the blur. A larger number will take longer to process. Must be an odd number. Defaults to %(default)s", default=5, type=int)


def compress(video_file, output_file, downscale=False, intensity=5):
    # Check that intensity is an odd number greater than 0, otherwise, round it up
    intensity = int(intensity)
    if intensity < 1:
        intensity = 1
    if intensity % 2 == 0:
        intensity += 1

    # Open input file
    cap = cv2.VideoCapture(video_file)
    # Get input video dimensions
    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    # Get input video framerate
    frame_rate = int(cap.get(cv2.CAP_PROP_FPS))

    # Downscale dimensions, if requested
    smallest_side = min(frame_width, frame_height)
    if downscale:
        # Downscale factor should not upscale the video if it is already smaller than DOWNSCALE_DIM pixels
        downscale_factor = min(1, DOWNSCALE_DIM/smallest_side)
    else:
        downscale_factor = 1
    new_dimensions = (int(frame_width * downscale_factor), int(frame_height * downscale_factor))
    # Open output file
    out = cv2.VideoWriter(output_file, cv2.VideoWriter_fourcc(*'mp4v'), frame_rate, new_dimensions)

    # Downscale the video if appropriate
    do_resize = (new_dimensions != (frame_width, frame_height)) and downscale
    if downscale and not do_resize:
        print("Ignoring request to downscale because the video is too small.")
    # Read video file and process it
    for _ in progressbar.progressbar(range(int(cap.get(cv2.CAP_PROP_FRAME_COUNT)))):
        # Read each frame
        ret, frame = cap.read()
        # Break early if we can't read a frame for whatever reason
        if not ret:
            print("Error reading frame")
            break
        # Change dimensions to new_dimensions
        if do_resize:
            frame = cv2.resize(frame, new_dimensions)

        # Blur each frame using a median filter
        frame = cv2.medianBlur(frame, intensity)

        # Write each frame to the output file
        out.write(frame)

    cap.release()
    out.release()

if __name__ == "__main__":
    args = parser.parse_args()

    video_file = args.video
    output_file = args.output
    downscale = args.downscale
    intensity = args.intensity

    compress(video_file, output_file, downscale=downscale, intensity=intensity)
