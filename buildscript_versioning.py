
import requests
import sys
from os.path import basename
from platformio import util

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

config = configparser.ConfigParser()
config.read("platformio.ini")


FILENAME_BUILDNO = 'versioning'
FILENAME_VERSION_H = 'include/version.h'
release_sub_version = config.get("common", "release_version") + '.' + config.get("common", "release_sub_version")
version = 'v' + release_sub_version

import datetime

build_no = 0
try:
    with open(FILENAME_BUILDNO) as f:
        build_no = int(f.readline()) + 1
except:
    print('Starting build number from 1..')
    build_no = 1
with open(FILENAME_BUILDNO, 'w+') as f:
    f.write(str(build_no))
    print('Build number: {}'.format(build_no))

hf = """
#ifndef BUILD_NUMBER
  #define BUILD_NUMBER "{}"
#endif
#ifndef VERSION
  #define VERSION "{} - {}"
#endif
#ifndef VERSION_SHORT
  #define VERSION_SHORT "{}"
#endif
#ifndef RELEASE_VERSION_SUB_VERSION
  #define RELEASE_VERSION_SUB_VERSION "{}"
#endif
""".format(build_no, version+str(build_no), datetime.datetime.now(), version+str(build_no),release_sub_version)
with open(FILENAME_VERSION_H, 'w+') as f:
    f.write(hf)


Import('env')
print (version+str(build_no))

env.Replace(PROGNAME="firmware_%s" % version+str(build_no))