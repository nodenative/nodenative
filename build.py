#!/usr/bin/env python

# source https://github.com/libuv/libuv/blob/master/gyp_uv.py
import glob
import platform
import os
import subprocess
import sys
import time

try:
    import multiprocessing.synchronize
    gyp_parallel_support = True
except ImportError:
     gyp_parallel_support = False

CC = os.environ.get('CC', 'cc')
script_dir = os.path.dirname(__file__)
project_root = os.path.normpath(script_dir)
gyps_dir = os.path.join(project_root, 'build')
output_dir = os.path.join(os.path.abspath(project_root), 'out')

root_gyp_lib = os.path.join(project_root, 'build', 'gyp')

if(not os.path.exists(root_gyp_lib)):
    try:
        print('build/gyp is missing, trying to clone from https://chromium.googlesource.com/external/gyp.git ...')
        proc = subprocess.Popen(['git', 'clone', 'https://chromium.googlesource.com/external/gyp.git', root_gyp_lib], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        res = proc.communicate();
        if(res[1] and 'error' in res[1]):
            print('error when trying to clone build/gyp:' + res[1])
            sys.exit(42)
        print('build/gyp imported')
    except OSError:
        print('You need to install gyp in build/gyp first. See the README.')
        sys.exit(42)

sys.path.insert(0, os.path.join(root_gyp_lib, 'pylib'))

try:
    import gyp
except ImportError:
    print('error to import gyp')
    exit(42)

def host_arch():
    machine = platform.machine()
    if machine == 'i386': return 'ia32'
    if machine == 'x86_64': return 'x64'
    if machine.startswith('arm'): return 'arm'
    if machine.startswith('mips'): return 'mips'
    return machine  # Return as-is and hope for the best.

def compiler_version():
    proc = subprocess.Popen(CC.split() + ['--version'], stdout=subprocess.PIPE)
    is_clang = 'clang' in proc.communicate()[0].split('\n')[0]
    proc = subprocess.Popen(CC.split() + ['-dumpversion'], stdout=subprocess.PIPE)
    version = proc.communicate()[0].split('.')
    version = map(int, version[:2])
    version = tuple(version)
    return (version, is_clang)

def run_gyp(args):
    rc = gyp.main(args)
    if rc != 0:
        print 'Error running GYP'
        sys.exit(rc)

def which(program):
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def gyp_generate(args):
    # GYP bug.
    # On msvs it will crash if it gets an absolute path.
    # On Mac/make it will crash if it doesn't get an absolute path.
    if sys.platform == 'win32':
        args.append(os.path.join(gyps_dir, 'all.gyp'))
        common_fn  = os.path.join(gyps_dir, 'common.gypi')
        options_fn = os.path.join(gyps_dir, 'options.gypi')
        # we force vs 2013 over others which would otherwise be the default for
        # gyp.
        # TODO: to decide
        if not os.environ.get('GYP_MSVS_VERSION'):
            os.environ['GYP_MSVS_VERSION'] = '2013'
    else:
        args.append(os.path.join(os.path.abspath(gyps_dir), 'all.gyp'))
        common_fn  = os.path.join(os.path.abspath(gyps_dir), 'common.gypi')
        options_fn = os.path.join(os.path.abspath(gyps_dir), 'options.gypi')

    if os.path.exists(common_fn):
        args.extend(['-I', common_fn])

    if os.path.exists(options_fn):
        args.extend(['-I', options_fn])

    args.append('--depth=' + project_root)

    # There's a bug with windows which doesn't allow this feature.
    if sys.platform != 'win32':
        if '-f' not in args:
            args.extend('-f make'.split())
        if 'eclipse' not in args and 'ninja' not in args:
            args.extend(['-Goutput_dir=' + output_dir])
            args.extend(['--generator-output', output_dir])
        (major, minor), is_clang = compiler_version()
        args.append('-Dgcc_version=%d' % (10 * major + minor))
        args.append('-Dclang=%d' % int(is_clang))

    if not any(a.startswith('-Dhost_arch=') for a in args):
        args.append('-Dhost_arch=%s' % host_arch())

    if not any(a.startswith('-Dtarget_arch=') for a in args):
        args.append('-Dtarget_arch=%s' % host_arch())

    if not any(a.startswith('-Dnode_byteorder=') for a in args):
        args.append('-Dnode_byteorder=%s' % sys.byteorder)

    if not any(a.startswith('-Dnnative_use_openssl=') for a in args):
        args.append('-Dnnative_use_openssl=true')

    if not any(a.startswith('-Dnnative_shared_openssl=') for a in args):
        args.append('-Dnnative_shared_openssl=false')

    if not any(a.startswith('-Dopenssl_no_asm=') for a in args):
        args.append('-Dopenssl_no_asm=false')

    if not any(a.startswith('-Dopenssl_fips=') for a in args):
        args.append('-Dopenssl_fips=""')

    if not any(a.startswith('-Dnnative_target_type=') for a in args):
        args.append('-Dnnative_target_type=static_library')

    if not any(a.startswith('-Duv_library=') for a in args):
        args.append('-Duv_library=static_library')

    if not any(a.startswith('-Dlibrary=') for a in args):
        args.append('-Dlibrary=static_library')

    if not any(a.startswith('-Dcomponent=') for a in args):
        args.append('-Dcomponent=static_library')


    # Some platforms (OpenBSD for example) don't have
    # multiprocessing.synchronize
    # so gyp must be run with --no-parallel
    if not gyp_parallel_support:
        args.append('--no-parallel')

    gyp_args = list(args)
    print gyp_args
    run_gyp(gyp_args)

if __name__ == '__main__':
    start = time.time()
    args = sys.argv[1:]
    generatorNames = ['gyp', 'gn']
    builderNames = ['ninja', 'make']
    generatorName = None
    builderName = None
    if '--generator' not in args:
        for i in generatorNames:
            if i == 'gyp':
                generatorName = i
                break
            elif which(i):
                generatorName = i
                break
    if '-f' not in args:
        for i in builderNames:
            if which(i):
                builderName = i
                break
        args.extend(['-f', i])
    else:
        builderName = args[args.index('-f')+1]
    print('generator: {0}, builder: {1}'.format(generatorName, builderName))

    if(generatorName == 'gyp'):
        gyp_generate(args)

    print('generate time {0}'.format(time.time() - start))
    print('build...')
    start_build = time.time()

    if builderName == 'ninja':
        subprocess.call([builderName, '-C', 'out/Debug'])
        print('build in {0}'.format(time.time() - start_build))
        start_build = time.time()
        subprocess.call([builderName, '-C', 'out/Release'])
        print('build in {0}'.format(time.time() - start_build))
    if builderName == 'make':
        subprocess.call([builderName, '-C', 'out'])
        print('build in {0}'.format(time.time() - start_build))
    else:
        print('unknown builder name {0}. Build it manually.'.format(builderName))
        sys.exit(42)

