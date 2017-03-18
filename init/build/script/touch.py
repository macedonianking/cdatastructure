#!python

import argparse

def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("--output", type=str, 
        help="Specify the output file")
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_arguments()
    open(args.output, "w").close()
    pass
