# -*- encoding: utf-8 -*-
'''
Created on 2017年5月14日

@author: lhb
'''
import os.path
import sys


def ResolveRspLinks(paths):
    """"""
    result = set()
    for item in paths:
        if item.startswith("@"):
            with open(item[1:], "r") as fileobj:
                result.update(map(lambda x:x.strip(), fileobj.readlines()))
        else:
            result.add(item)
    return result

def CombineResourceWhitelists(whitelist_candinates, outfile):
    """"""
    resources = set()
    for candinate in whitelist_candinates:
        whitelist_path = os.path.join(candinate, ".whitelist")
        if not os.path.exists(whitelist_path):
            continue
        with open(whitelist_path, "r") as fileobj:
            resources.update(fileobj.readlines())
        pass
    # 写入新的资源白名单
    with open(outfile, "w") as fileobj:
        fileobj.writelines(resources)
    pass

def GetPythonDependencies():
    python_modules = []
    for m in sys.modules.values():
        if hasattr(m, "__file__") and not m.__file__.endswith("pyc"):
            python_modules.append(m.__file__)
    python_modules = sorted(python_modules)
    return sorted(python_modules)

def WriteDepfile(depfile, first_gn_output, inputs=None, add_pydep=False):
    inputs = inputs or []
    if add_pydep:
        inputs += GetPythonDependencies()

    with open(depfile, mode='w', encoding='utf-8') as fileobj:
        fileobj.write(first_gn_output.replace(" ", "\\ "))
        fileobj.write(": ")
        fileobj.write(" ".join([v.replace(" ", "\\ ") for v in inputs]))
        fileobj.write("\n")
    pass
