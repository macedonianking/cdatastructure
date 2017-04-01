#! python

import sys
import optparse
import itertools

from util import build_utils

_ROOT_TYPES = [ 'android_apk', 'deps_dex', 'java_binary', 'resource_rewriter' ]

class Deps(object):
    def __init__(self, direct_deps_config_paths):
        pass;
    pass;

dep_config_cache = {}
def GetDepConfig(path):
    """得到path对应的文件的deps_info
    """
    if not path in dep_config_cache:
        dep_config_cache[path] = build_utils.ReadJson(path)['deps_info'];
    return dep_config_cache[path];

def GetAllDepsConfigsInOrder(deps_config_paths):
    """deps_configs_paths是build_config的任务列表
    返回所有的build_config的依赖任务
    """
    def GetDeps(path):
        return set(GetDepConfig(path)['deps_configs'])
    pass

def DepsOfType(wanted_type, configs):
    """返回configs里面type和wanted_type相同的设置
    """
    return [c for c in configs if wanted_type == c['type']];

def _create_list(asset_map):
    ret = [ '%s:%s' % (dst, src) for dst, src in asset_map.iteritems() ]
    ret.sort()
    return ret

def _MergeAssets(all_assets):
    '''Merge all assets.
    Returns a tuple of lists. (compressed, uncompressed)
    '''
    compressed = {}
    uncompressed = {}
    for asset_dep in all_assets:
        entry = asset_dep['assets']
        disable_compression = entry.get('disable_compression', False)
        if disable_compression:
            # 不压缩
            dst_map = uncompressed
            rhs_map = compressed
        else:
            # 压缩
            dst_map = compressed
            rhs_map = uncompressed

        outputs = entry.get('outputs', [])
        for src, dst in itertools.izip_longest(entry['sources'], outputs):
            if not dst:
                dst = os.path.basename(src)
            dst_map[dst] = src
            rhs_map.pop(dst, 0)
    return _create_list(compressed), _create_list(uncompressed)

def _ResultGroups(configs):
    """从configs中获取group的对象并内联起来
    group类型的build_config里面有deps_configs对应新的build_config文件的路径
    """
    ret = list(configs)
    while True:
        groups = DepsOfType('group', ret)
        if not groups:
            return ret
        for config in groups:
            index = ret.index(config)
            expanded_configs = [ GetDepConfig(p) for p in config['deps_configs'] ]
            ret[index:index + 1] = expanded_configs
    pass

def _FilterDepsPaths(deps_configs, target_type):
    """
    deps_configs是从"deps_configs"字段读取的build_config文件的路径
    target_type对本目标的类型
    """
    configs = [ GetDepConfig(p) for p in deps_configs ]
    configs = _ResultGroups(configs) # 内敛group目标类型
    # root目标的类型不作为deps
    configs = [ c for c in configs if c['type'] not in _ROOT_TYPES ]
    return [ c['path'] for c in configs ]


def main(argv):
    parser = optparse.OptionParser()
    build_utils.AddDepfileOption(parser)
    parser.add_option('--build-config', 'Path to build_config option.')
    parser.add_option(
        '--type',
        help='Type of this target(e.g. android_library).')
    parser.add_option(
        '--deps-configs',
        help='List of paths for dependency\'s build_config files.')

    # android_resources options
    parser.add_option('--srcjar', help='Path to target\'s resources srcjar.')
    parser.add_option('--resource-zip', help='Path to target\'s resource zip.')
    parser.add_option('--r-txt', help='Path to target\'s R.txt file.')
    parser.add_option('--android-manifest', help='Path to android manifest.')
    parser.add_option('--package-name', help='Java package name for these resources.')
    parser.add_option('--resource-dirs', action='append', default=[], 
                      help='GYP-list of resource dirs')

    # android_assets options.
    parser.add_option('--asset-sources', help='List of asset sources')
    parser.add_option('--disable-asset-compression', 
                      action='store_true', help='Whether to disable asset compression.')

    # java_library options.
    parser.add_option('--jar-path', help='Target\'s jar output path.')
    parser.add_option('--java-sources-file', help='Path to .sources file.')

    options, args = parser.parse_args(argv)
    if args:
        parser.error('No position ')

    return 0;

if __name__ == "__main__":
    sys.exit(main(sys.argv));
    pass;
