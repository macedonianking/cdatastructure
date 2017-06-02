#!/usr/bin/env python
#
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Archives a set of files.
"""
import argparse
import os
import shutil
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, 'gyp'))
from util import build_utils


def main():
    parser = argparse.ArgumentParser()
    build_utils.AddDepfileOption(parser)

    parser.add_argument('--inputs', help='List of files archives.')
    parser.add_argument('--output', help='The output text file to generate.')
    parser.add_argument('--base-dir',
                        help='If provided, the paths in the archive will be '
                             'relative to this directory', default='.')

    options = parser.parse_args()
    build_utils.CheckOptions(options, parser, required=['inputs', 'output', 'base_dir'])

    # 解析inputs参数
    options.inputs = build_utils.ParseGnList(options.inputs)

    # 删除原来的内容
    build_utils.MakeDirectory(options.base_dir)
    for item in os.listdir(options.base_dir):
        path = os.path.join(options.base_dir, item)
        if os.path.isdir(path):
            shutil.rmtree(path)
        else:
            os.remove(path)

    # 释放所有的对象
    for zip_file in options.inputs:
        build_utils.ExtractAll(zip_file, options.base_dir)

    if options.depfile:
        build_utils.WriteDepfile(options.depfile, first_gn_output=options.output, inputs=options.inputs)

    build_utils.Touch(options.output)


if __name__ == '__main__':
    sys.exit(main())
