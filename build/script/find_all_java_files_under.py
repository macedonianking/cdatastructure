#!python

import argparse;
import subprocess;
import os;
from os.path import isdir;

"""parse the runtime arguments"""
def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("--dir", dest="dir_path", type=str);
    parser.add_argument("--relative-dir", dest="relative_dir", type=str);
    return parser.parse_args();

"""得到结果文件名，格式化到对应的目录下面"""
def get_result_path(file_path, relative_path):
    path_text = os.path.relpath(file_path, relative_path);
    path_text = path_text.replace("\\", "/");
    if path_text.endswith(".java"):
        return path_text;
    return None;

"""The main entrance"""
def main():
    args = parse_arguments();
    if not isdir(args.dir_path) or not isdir(args.relative_dir):
        os.exit(1);

    for root, dirs, files in os.walk(args.dir_path):
        for name in files:
            text = get_result_path(os.path.join(root, name), args.relative_dir);
            if text:
                print(text);
    pass;

if __name__ == "__main__":
    main();
    pass;
