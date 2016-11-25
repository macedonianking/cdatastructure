import argparse
import subprocess;

def parse_arguments():
    parser = argparse.ArgumentParser();
    parser.add_argument("-o", type=str, help="specify the output file");
    parser.add_argument("source");
    return parser.parse_args();

def main():
    args = parse_arguments();
    command = "cp -f %s %s" % (args.source, args.o);
    subprocess.check_output(command, shell=True);
    pass;

if __name__ == "__main__":
    main();
    pass;
