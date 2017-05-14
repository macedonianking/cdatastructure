'''
Created on 2017年5月14日

@author: lhb
'''
import os.path


def ResolveRspLinks(paths):
    """"""
    result = set()
    for item in paths:
        if item.startswith("@"):
            with open(item, "r") as fileobj:
                result.update(map(lambda x:x.strip(), fileobj.readlines()))
        else:
            result.add(item)
    pass

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
