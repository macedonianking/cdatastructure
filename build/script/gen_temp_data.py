
import sys
import os.path

def _createTempFile(path):
    if os.path.isfile(path):
        os.remove(path)
    with open(path, mode="w+") as tempFile:
        pass;

def main(path):
    _createTempFile(path);
    return 0;

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
