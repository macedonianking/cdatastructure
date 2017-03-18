#!python

import argparse;
import subprocess;

"""parse the runtime arguments"""
def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("--out", type=str);
    parser.add_argument("--classes-dir", dest="classes_dir", type=str);
    parser.add_argument("--javac-path", dest="javac_path", type=str);
    parser.add_argument("--depsfile", dest="depsfile", type=str);
    parser.add_argument("sources", nargs="*");
    return parser.parse_args();

def show_arg_parse(args):
    print(args.sources);
    print(args.out);
    print(args.classes_dir);
    print(args.javac_path);
    print(args.depsfile);
    pass;

"""The main entrance"""
def main():
    args = parse_arguments();
    with open(args.depsfile, "w+") as fileObj:
        for name in args.sources:
            print(name, file = fileObj);
    with open(args.out, "w+") as fileObj:
        pass;
    pass;

if __name__ == "__main__":
    main();
    pass;
