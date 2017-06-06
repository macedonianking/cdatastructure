# -*- encoding: utf-8 -*-
'''
Created on 2017年5月15日

@author: lhb
'''
import argparse
import os
import subprocess
import sys


def CollectTOC(options):
    toc = [options.soname + "\n"]

    command = "%s -g %s" % (options.nm, options.sofile)
    child = subprocess.Popen(command, stdout=subprocess.PIPE, universal_newlines=True)
    toc += child.stdout.readlines()
    return child.wait(), toc

def UpdateTOC(tocfile, toc):
    old_toc = None
    if os.path.exists(tocfile):
        with open(tocfile, "r", encoding="utf-8") as fileobj:
            old_toc = list(fileobj.readlines())

    if old_toc == toc:
        return

    with open(tocfile, mode="w", encoding="utf-8") as fileobj:
        fileobj.writelines(toc)
    pass

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--nm",
                        required=True,
                        help="Specify the nm path.")
    parser.add_argument("--soname",
                        required=True)
    parser.add_argument("--tocfile",
                        required=True)
    parser.add_argument("--sofile",
                        required=True,
                        metavar="SONAME",
                        help="Specify the shared library's path.")
    parser.add_argument("--output",
                        required=True,
                        metavar="PATH")
    parser.add_argument("command",
                        metavar="COMMAND",
                        help="Linking command.")
    options = parser.parse_args()

    return_code = subprocess.call(options.command, universal_newlines=True)
    if return_code:
        return return_code

    result, toc = CollectTOC(options)
    if result:
        return result

    UpdateTOC(options.tocfile, toc)
    return 0

if __name__ == '__main__':
    sys.exit(main())
