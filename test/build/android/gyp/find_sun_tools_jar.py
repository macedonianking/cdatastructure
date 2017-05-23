#!/usr/bin/env python
#
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""This finds the java distribution's tools.jar and copies it somewhere.
"""

import argparse
import os
import re
import shutil
import sys

from util import build_utils

RT_JAR_FINDER = re.compile('\\[Opened\\s+(.*rt.jar)\\]')


def main():
    parser = argparse.ArgumentParser(description='Find Sun Tools Jar')
    parser.add_argument('--depfile',
                        help='Path to depfile. This must be specified as the '
                             'action\'s first output.')
    parser.add_argument('--output', required=True)
    args = parser.parse_args()

    sun_tools_jar_path = FindSunToolsJarPath()
    if not sun_tools_jar_path:
        sun_tools_jar_path = FindSunToolsPathFromJavaHome()

    if sun_tools_jar_path is None:
        raise Exception("Couldn\'t find tools.jar")

    # Using copyfile instead of copy() because copy() calls copymode()
    # We don't want the locked mode because we may copy over this file again
    shutil.copyfile(sun_tools_jar_path, args.output)

    if args.depfile:
        build_utils.WriteDepfile(args.depfile, args.output, [sun_tools_jar_path])


def FindSunToolsJarPath():
    # This works with at least openjdk 1.6, 1.7 and sun java 1.6, 1.7
    stdout = build_utils.CheckOutput(
        ["java", "-verbose", "-version"], print_stderr=False)
    rt_path = None
    for ln in stdout.splitlines():
        match = RT_JAR_FINDER.match(ln)
        if match:
            rt_path = match.group(1)
    if not rt_path:
        return None

    tools_jar_path = os.path.join(os.path.dirname(rt_path), os.pardir, os.pardir, "lib", "tools.jar")
    tools_jar_path = os.path.abspath(tools_jar_path)
    if os.path.isfile(tools_jar_path):
        return tools_jar_path
    return None


def FindSunToolsPathFromJavaHome():
    java_home = os.getenv('JAVA_HOME', None)
    if not java_home:
        return None

    tools_jar = os.path.abspath(os.path.join(java_home, "lib", "tools.jar"))
    return tools_jar


if __name__ == '__main__':
    sys.exit(main())
