
import sys
import os.path

import argparse

def createFinalApk(path):
    if os.path.isfile(path):
        os.remove(path)
    with open(path, mode="w+") as tempFile:
        pass;

def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--final-apk", help="Specify the final apk path.");
    parser.add_argument("--build-config", 
        help="Specify the build_config file to be used to generate the final apk.");
    return parser.parse_args()

def main(path):
    args = parseArguments()
    createFinalApk(args.final_apk)
    return 0;

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
    pass
