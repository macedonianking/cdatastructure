# -*- encoding: utf-8 -*-

import argparse
import fnmatch
import os
import sys

_path = os.path.join(os.path.dirname(__file__), "android", "gyp")
sys.path.append(os.path.abspath(_path))

def ParseGnList(gn_string):
    """Converts a command-line parameter into a list.
  
    If the input starts with a '[' it is assumed to be a GN-formatted list and
    it will be parsed accordingly. When empty an empty list will be returned.
    Otherwise, the parameter will be treated as a single raw string (not
    GN-formatted in that it's not assumed to have literal quotes that must be
    removed) and a list will be returned containing that string.
  
    The common use for this behavior is in the Android build where things can
    take lists of @FileArg references that are expanded via ExpandFileArgs.
    """
    if gn_string.startswith('['):
        parser = gn_helpers.GNValueParser(gn_string)
        return parser.ParseList()
    if len(gn_string):
        return [gn_string]
    return []


def _ParseOptions(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--include-patterns",
                        help="Specify the include file patterns.")
    parser.add_argument("--exclude-patterns",
                        help="Specify the exclude file patterns.")
    parser.add_argument("--non-recursive",
                        action="store_true",
                        default=False,
                        help="Specify the recursively search.")
    parser.add_argument("--dir", required=True,
                        help="Specify the directory to search.")
    parser.add_argument("--relative-dir",
                        help="Specify the directory the result output.")

    options = parser.parse_args(argv)
    if not options.relative_dir:
        options.relative_dir = options.dir

    if options.include_patterns:
        options.include_patterns = ParseGnList(options.include_patterns)
    if options.exclude_patterns:
        options.exclude_patterns = ParseGnList(options.exclude_patterns)
    return options, parser


def FindInDirectory(path, relative_dir, filter_func):
    result = []
    for _root, _dirs, _files in os.walk(path):
        paths = [os.path.relpath(os.path.join(_root, x), relative_dir) for x in _files]
        result.extend([x for x in paths if filter_func(x)])
    return result


def FindInDirectoryNonRecursively(path, relative_dir, filter_func):
    result = []
    for _name in os.listdir(path):
        rel_name = os.path.relpath(os.path.join(path, _name), relative_dir)
        if filter_func(rel_name):
            result.append(rel_name)
    return result


def main(argv):
    options, _ = _ParseOptions(argv)

    def _Filter(path):
        if options.include_patterns and not any(map(lambda x: fnmatch.fnmatch(path, x), options.include_patterns)):
            return False
        if options.exclude_patterns and any(map(lambda x: fnmatch.fnmatch(path, x), options.exclude_patterns)):
            return False
        return True

    if options.non_recursive:
        result = FindInDirectoryNonRecursively(options.dir, options.relative_dir, _Filter)
    else:
        result = FindInDirectory(options.dir, options.relative_dir, _Filter)
    result = sorted(result)
    for item in result:
        print(item.replace("\\", "/"))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
