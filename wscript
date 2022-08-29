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

APPNAME='jpl'
VERSION='0-dev'

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

    opt.add_option('--mandir', type='string', help="Manpage directory [Default: <prefix>/share/man]")

def configure(conf):
    conf.load('compiler_c')

    conf.check_cfg(package='libevent', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='liblo', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='dbus-1', mandatory=False, args='--cflags --libs')

    if Options.options.mandir:
        conf.env['MANDIR'] = Options.options.mandir
    else:
        conf.env['MANDIR'] = conf.env['PREFIX'] + '/share/man'

    conf.define('JPL_VERSION', VERSION)
    conf.define('HAVE_GITVERSION_H', 1)
    conf.define('BUILD_TIMESTAMP', time.ctime())
    conf.write_config_header('config.h')

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
    print()

def build(bld):
    bld(rule=git_ver,
        target='gitversion.h',
        update_outputs=True,
        always=True,
        ext_out=['.h'])

    prog = bld(features=['c', 'cprogram'])
    prog.source = [
        'main.c',
        ]
    prog.includes = '.' # config.h, gitverson.h include path
    prog.target = 'jpl'
    prog.use = ['LIBEVENT', 'LIBLO', 'DBUS-1']
    prog.defines = ["HAVE_CONFIG_H"]

    # install man pages
    man_pages = [
#TODO:        "jpl.1",
        ]

    for i in range(len(man_pages)):
        man_pages[i] = "man/" + man_pages[i]

    bld.install_files(os.path.join(bld.env['MANDIR'], 'man1'), man_pages)
