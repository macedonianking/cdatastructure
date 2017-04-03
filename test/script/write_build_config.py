
import sys
import os.path

import argparse
import dep_file

_deps_config = []

def createBuildConfig(path):
    if os.path.isfile(path):
        os.remove(path)
    with open(path, mode="w+") as tempFile:
        pass;

def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--type", help="Specify the build config type.");
    parser.add_argument("--build-config", 
        help="Specify the build_config file to be used to generate the final apk.");
    parser.add_argument("--deps-config",
        help="Specify the build config files that project needs.")
    parser.add_argument("--depfile",
        help="Sepcify the depfile that command generate.")
    parser.add_argument("--required-android",
        action="store_true", help="Sepecify whether this project needs android.jar.")
    return parser.parse_args()

def main(path):
    args = parseArguments()
    _deps_config = eval(args.deps_config)
    dep_file.createDepFile(args.depfile, args.build_config, _deps_config)
    createBuildConfig(args.build_config)
    return 0;

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
    pass
