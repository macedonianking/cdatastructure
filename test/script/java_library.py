import sys
import os.path

import argparse

def createFinalJar(path):
    if os.path.isfile(path):
        os.remove(path)
    with open(path, mode="w+") as tempFile:
        pass;

def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--jar", help="Specify the output java jar path.");
    parser.add_argument("--build-config", 
        help="Specify the build_config file to be used to generate the Java jar.");
    return parser.parse_args()

def main():
    args = parseArguments()
    createFinalJar(args.jar)
    return 0;

if __name__ == "__main__":
    sys.exit(main())
    pass
