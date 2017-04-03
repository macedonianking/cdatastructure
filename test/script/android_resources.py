import sys
import os.path

import argparse

def createResourcesZip(path):
    if os.path.isfile(path):
        os.remove(path)
    with open(path, mode="w+") as tempFile:
        pass;

def createResourcesDepfile(depfile, manifest, resources_dir,
    extra_file):
    """
    位android_resources目标创建depfile文件，写入manifest的路径，
    resources_dir下面的所有路径和extra_file"""
    with open(depfile, "w+") as fileObj:
        print("%s:" % (depfile), file=fileObj);
    pass;

def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--resources-zip", help="Specify the resources.zip path.");
    parser.add_argument("--R-jar", help="Specify the resources R.jar path.");
    parser.add_argument("--manifest", help="Specify the AndroidManifest.xml path");
    parser.add_argument("--build-config", 
        help="Specify the build_config file to be used to build the resources target.");
    parser.add_argument("--resources-dir", 
        help="Specify the resources directories.")
    parser.add_argument("--depfile", 
        help="Specify the dpefile path.")
    return parser.parse_args()

def main(path):
    args = parseArguments()
    createResourcesZip(args.final_apk)
    return 0;

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
    pass
