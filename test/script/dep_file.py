
class DepfileWriter:
    """DepfileWriter defined to help write depfile.
    """
    def __init__(self, depfile, destfile):
        """
        depfile:    The path of the depfile.
        destfile:   The path of the destination file.
        """
        self._M_file = open(depfile, "w+")
        self._M_is_closed = False
        print("%s:" % (destfile), file = self._M_file)
        pass;

    def add(self, argv):
        """Add argv paths to the depfile.
        *argv:  The paths to be writen to the depfile file.
        """
        for path in argv:
            print("    ", end='', file = self._M_file)
            print(path, file = self._M_file)
        pass;

    def close(self):
        """关闭打开的文件
        """
        if not self._M_is_closed:
            self._M_is_closed = True
            self._M_file.close()
            self._M_file = None
        pass;


def createDepFile(path, destFile, argv):
    """创建依赖文件
    """
    writer = DepfileWriter(path, destFile)
    writer.add(argv)
    writer.close()
    pass
