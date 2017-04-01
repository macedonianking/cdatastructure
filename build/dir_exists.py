#!python
import os.path
import sys

def main():
  sys.stdout.write(_is_dir(sys.argv[1]))
  return 0

def _is_dir(dir_name):
  return str(os.path.isdir(dir_name))

if __name__ == '__main__':
  sys.exit(main())
