#! pytyon

import json

"""Add parser depfile file option
"""
def AddDepfileOption(parser):
    if hasattr(parser, "add_option"):
        func = parser.add_option
    else
        func = parser.add_argument
    func("--depfile", help='Path to depfile (refer to `gn help depfile`)')
    pass;

def CheckOptions(options, parser, required = None):
    if not required:
        return;
    for option_name in required:
        if getattr(options, option_name) is None:
            parser.error('--%s is required.' % option_name.replace('_', '-'))
    pass

def ReadJson(path):
    """读取path目录对应的文件转化成json对象
    """
    with open(path, 'r') as jsonFile:
        return json.load(jsonFile)
    pass



def GetSortedTransitiveDependencies(top, get_deps):
    """top是一个列表,get_deps是得到对应的配置关系
    Args:
        top a list
        get_deps a function
    Return:
    返回一个list
    """
    # First all deps
    
