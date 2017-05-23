import argparse
import fnmatch
import os
import sys

_path = os.path.join(os.path.dirname(__file__), "android", "gyp")
sys.path.append(os.path.abspath(_path))

from util import build_utils


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
        options.include_patterns = build_utils.ParseGnList(options.include_patterns)
    if options.exclude_patterns:
        options.exclude_patterns = build_utils.ParseGnList(options.exclude_patterns)
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
