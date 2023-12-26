#! /usr/bin/env python
# encoding: utf-8
#
# wscript for building JACK Plugin Launcher

from __future__ import print_function

import os
import subprocess
import shutil
import re
import time

from waflib import Logs, Options, TaskGen, Context, Utils
from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

from waftoolchainflags import WafToolchainFlags

APPNAME='jpl'
VERSION='0-dev'
DBUS_NAME_BASE = 'org.ladish.jpl'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

def git_ver(self):
    bld = self.generator.bld
    header = self.outputs[0].abspath()
    if os.access('./version.h', os.R_OK):
        header = os.path.join(os.getcwd(), out, "version.h")
        shutil.copy('./version.h', header)
        data = open(header).read()
        m = re.match(r'^#define GIT_VERSION "([^"]*)"$', data)
        if m != None:
            self.ver = m.group(1)
            Logs.pprint('BLUE', "tarball from git revision " + self.ver)
        else:
            self.ver = "tarball"
        return

    if bld.srcnode.find_node('.git'):
        self.ver = bld.cmd_and_log("LANG= git rev-parse HEAD", quiet=Context.BOTH).splitlines()[0]
        if bld.cmd_and_log("LANG= git diff-index --name-only HEAD", quiet=Context.BOTH).splitlines():
            self.ver += "-dirty"

        Logs.pprint('BLUE', "git revision " + self.ver)
    else:
        self.ver = "unknown"

    fi = open(header, 'w')
    if self.ver != "unknown":
        fi.write('#define GIT_VERSION "%s"\n' % self.ver)
    fi.close()

def display_msg(conf, msg="", status = None, color = None):
    if status:
        #Logs.pprint(msg, status, color)
        conf.msg(msg, status, color=color)
    else:
        Logs.pprint('NORMAL', msg)

def display_raw_text(conf, text, color = 'NORMAL'):
    Logs.pprint(color, text, sep = '')

def display_line(conf, text, color = 'NORMAL'):
    Logs.pprint(color, text, sep = os.linesep)

def options(opt):
    # options provided by the modules
    opt.load('compiler_c')
    opt.load('wafautooptions')

    opt.add_option('--debug', action='store_true', default=False, dest='debug', help="Build debuggable binaries")
    opt.add_auto_option(
        'devmode',
        help='Enable devmode', # enable warnings and treat them as errors
        conf_dest='BUILD_DEVMODE',
        default=False,
    )

    opt.add_option('--mandir', type='string', help="Manpage directory [Default: <prefix>/share/man]")

def configure(conf):
    conf.load('compiler_c')
    conf.load('wafautooptions')

    conf.env['BUILD_DEBUG'] = Options.options.debug

    flags = WafToolchainFlags(conf)

    conf.check_cfg(package='libevent', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='liblo', mandatory=True, args='--cflags --libs')

    conf.check_cfg(package='dbus-1', mandatory=False, args='--cflags --libs')

    dbus_dir = conf.check_cfg(package='dbus-1', args='--variable=session_bus_services_dir', msg="Retrieving D-Bus services dir")
    if not dbus_dir:
        dbus_dir = os.path.join(os.path.normpath(conf.env['PREFIX']), 'share', 'dbus-1', 'services')

    dbus_dir = dbus_dir.strip()
    conf.env['DBUS_SERVICES_DIR'] = dbus_dir

    conf.check_cfg(package='cdbus-1', mandatory=False, args='--cflags --libs')

    if Options.options.mandir:
        conf.env['MANDIR'] = Options.options.mandir
    else:
        conf.env['MANDIR'] = conf.env['PREFIX'] + '/share/man'

    conf.env['LIBEXEC_DIR'] = conf.env['PREFIX'] + '/libexec/'

    conf.define('JPL_VERSION', VERSION)
    conf.define('HAVE_GITVERSION_H', 1)
    conf.define('DBUS_NAME_BASE', DBUS_NAME_BASE)
    conf.define('DBUS_BASE_PATH', '/' + DBUS_NAME_BASE.replace('.', '/'))
    conf.define('BASE_NAME', APPNAME)
    conf.define('BUILD_TIMESTAMP', time.ctime())
    conf.write_config_header('config.h')

    flags.add_c('-std=gnu99')
    flags.add_c('-fvisibility=hidden')
    if conf.env['BUILD_DEVMODE']:
        flags.add_c(['-Wall', '-Wextra'])
        #flags.add_c('-Wpedantic')
        #flags.add_c('-Werror')
        flags.add_c('-Wno-unused-parameter') # FIXME

        # https://wiki.gentoo.org/wiki/Modern_C_porting
        if conf.env['CC'] == 'clang':
            flags.add_c('-Wno-unknown-argumemt')
            flags.add_c('-Werror=implicit-function-declaration')
            flags.add_c('-Werror=incompatible-function-pointer-types')
            flags.add_c('-Werror=deprecated-non-prototype')
            flags.add_c('-Werror=strict-prototypes')
            if int(conf.env['CC_VERSION'][0]) < 16:
                flags.add_c('-Werror=implicit-int')
        else:
            flags.add_c('-Wno-unknown-warning-option')
            flags.add_c('-Werror=implicit-function-declaration')
            flags.add_c('-Werror=implicit-int')
            flags.add_c('-Werror=incompatible-pointer-types')
            flags.add_c('-Werror=strict-prototypes')

    if conf.env['BUILD_DEBUG']:
        flags.add_c('-g')
        flags.add_c('-O0')
        flags.add_link('-g')

    flags.flush()

    gitrev = None
    if os.access('gitversion.h', os.R_OK):
        data = file('gitversion.h').read()
        m = re.match(r'^#define GIT_VERSION "([^"]*)"$', data)
        if m != None:
            gitrev = m.group(1)

    print()
    display_msg(conf, "==================")
    version_msg = "jpl-" + VERSION
    if gitrev:
        version_msg += " exported from " + gitrev
    else:
        version_msg += " git revision will checked and eventually updated during build"
    print(version_msg)
    print()

    display_msg(conf, "Install prefix", conf.env['PREFIX'], 'CYAN')
    display_msg(conf, "bin dir", conf.env['BINDIR'], 'CYAN')
    display_msg(conf, "libexec dir", conf.env['LIBEXEC_DIR'], 'CYAN')
    display_msg(conf, "man dir", conf.env['MANDIR'], 'CYAN')
    display_msg(conf, "D-Bus services dir", conf.env['DBUS_SERVICES_DIR'], 'CYAN')
    display_msg(conf, "Compiler", conf.env['CC'][0], 'CYAN')
    conf.summarize_auto_options()
    flags.print()
    print()

def create_service_taskgen(bld, target, opath, binary):
    bld(
        features     = 'subst', # the feature 'subst' overrides the source/target processing
        source       = ['dbus.service.in'], # list of string or nodes
        target       = target,  # list of strings or nodes
        install_path = bld.env['DBUS_SERVICES_DIR'] + os.path.sep,
        # variables to use in the substitution
        dbus_object_path = opath,
        daemon_bin_path  = binary)

def build(bld):
    bld(rule=git_ver,
        target='gitversion.h',
        update_outputs=True,
        always=True,
        ext_out=['.h'])

    lib = bld.shlib(source = [], features = 'c cshlib', includes = [bld.path.get_bld()])
    lib.vnum = "0.0.1"
    lib.uselib = 'DL'
    lib.target = 'jpl'
    for source in [
        'loader.c',
        'catdup.c',
        ]:
        lib.source.append(source)

    prog = bld(features=['c', 'cprogram'])
    prog.source = [
        'main.c',
        ]
    prog.includes = '.' # config.h, gitverson.h include path
    prog.target = 'jpl'
    prog.use = ['LIBEVENT', 'LIBLO', 'DBUS-1', 'CDBUS-1']
    prog.defines = ["HAVE_CONFIG_H"]

    daemon = bld(features=['c', 'cprogram'])
    daemon.source = [
        'jpld.c',
        'appman.c',
        ]
    daemon.includes = '.' # config.h, gitverson.h include path
    daemon.target = 'jpld'
    daemon.use = ['LIBEVENT', 'LIBLO', 'DBUS-1', 'CDBUS-1']
    daemon.defines = ["HAVE_CONFIG_H"]
    daemon.install_path = bld.env['LIBEXEC_DIR']

    # process dbus.service.in -> jpld.service
    create_service_taskgen(bld, DBUS_NAME_BASE + '.service', DBUS_NAME_BASE, os.path.join(daemon.install_path, daemon.target))

    # install man pages
    man_pages = [
#TODO:        "jpl.1",
        ]

    for i in range(len(man_pages)):
        man_pages[i] = "man/" + man_pages[i]

    bld.install_files(os.path.join(bld.env['MANDIR'], 'man1'), man_pages)
