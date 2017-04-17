#!python

import argparse;
import subprocess;

"""parse the runtime arguments"""
def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("-o", help="Path to the target's output file.");
    parser.add_argument("--ar-path", help="Path to ar executable path.")
    parser.add_argument("--arflags", help="Ar flags for this archive library.");
    parser.add_argument("sources", nargs="*");
    return parser.parse_args();

"""The main entrance"""
def main():
    args = parse_arguments();
    command = "rm -f %s" % (args.o);
    subprocess.check_output(command, shell=True);
    command = "%s %s %s %s" % (args.ar_path, args.arflags, args.o, " ".join(args.sources));
    subprocess.check_output(command, shell=True);
    pass;

if __name__ == "__main__":
    main();
    pass;
