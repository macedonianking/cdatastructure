# -*- encoding:utf-8 -*-
'''
Created on 2017年5月14日

@author: lhb
'''
import argparse
import os.path
import subprocess
import sys

import wrapper_utils


def main():
    # 添加参数的解析
    parser = argparse.ArgumentParser()
    parser.add_argument("--ar",
                        required=True,
                        help="The ar binary to run.")
    parser.add_argument("--depfile",
                        help="The depfile path.")
    parser.add_argument("--output",
                        required=True,
                        help="Output archive file.")
    parser.add_argument("--plugin",
                        help="Load plugin.")
    parser.add_argument("--resource-whitelist",
                        help="Merge all resource whitelists into a single file.")
    parser.add_argument("operations",
                        help="Operations on archive.")
    parser.add_argument("inputs",
                        nargs="+",
                        help="Input files.")

    options = parser.parse_args()
    if options.resource_whitelist:
        whitelist_candinates = wrapper_utils.ResolveRspLinks(options.inputs)
        wrapper_utils.CombineResourceWhitelists(whitelist_candinates, options.resource_whitelist)

    # 合成命令
    command = [options.ar, options.operations ]
    if options.plugin:
        command += ["--plugin", options.plugin]
    command.append(options.output)
    command += options.inputs

    # 先删除旧文件
    if os.path.exists(options.output):
        try:
            os.remove(options.output)
        except OSError as e:
            if e.errno != os.errno.ENOENT:
                raise
        pass

    return_code = subprocess.call(command, universal_newlines=True)
    if return_code:
        return return_code

    if options.depfile:
        # 忽略掉.rsp文件，应为rsp文件在任务执行完毕后被删除
        inputs = options.inputs
        inputs = [v for v in inputs if not (v.startswith("@") and v.endswith(".rsp"))]
        wrapper_utils.WriteDepfile(options.depfile, options.output, inputs=inputs,
                                   add_pydep=True)
    return 0

if __name__ == '__main__':
    sys.exit(main())
    pass
