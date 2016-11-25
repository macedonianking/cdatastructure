#!python

import argparse;
import subprocess;

"""parse the runtime arguments"""
def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("-o", type=str);
    parser.add_argument("--args", type=str);
    parser.add_argument("--ar", type=str);
    parser.add_argument("sources", nargs="*");
    return parser.parse_args();

"""The main entrance"""
def main():
    args = parse_arguments();
    command = "rm -f %s" % (args.o);
    subprocess.check_output(command, shell=True);
    command = "%s %s %s %s" % (args.ar, args.args, args.o, " ".join(args.sources));
    subprocess.check_output(command, shell=True);
    pass;

if __name__ == "__main__":
    main();
    pass;
