import sys
import argparse


def _ParseArgs(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, help="Specify the output path.")
    return parser.parse_args()

def main():
    options = _ParseArgs(sys.argv[1:])
    with open(options.output, mode="w", encoding="utf-8") as fileobj:
        fileobj.write("\n")
        pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
    pass