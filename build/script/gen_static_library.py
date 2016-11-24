#!python

import argparse;
import subprocess;

def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("-o", type=str);
    parser.add_argument("--args", type=str);
    parser.add_argument("sources", nargs="*");
    return parser.parse_args();

def main():
    args = parse_arguments();
    command = "rm -f %s" % (args.o);
    subprocess.check_output(command, shell=True);
    command = "ar %s %s %s" % (args.args, args.o, " ".join(args.sources));
    subprocess.check_output(command, shell=True);
    pass;

if __name__ == "__main__":
    main();
    pass;
