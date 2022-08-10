# Copyright (c) 2014-present PlatformIO <contact@platformio.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import requests
import sys
from os.path import basename
from platformio import util

Import('env')
# print(env)
# print(env.Dump())

Import("projenv") 
# print(projenv.Dump())


try:
    import configparser
except ImportError:
    import ConfigParser as configparser

config = configparser.ConfigParser()
config.read("platformio.ini")

#project_config = util.load_project_config()
#bintray_config = {k: v for k, v in project_config.items("bintray")}

#version = project_config.get("common", "release_version")
version = config.get("common", "release_version")
sub_version = config.get("common", "release_sub_version")
version_subversion=version+"."+sub_version

print("#######################################################################")
print("#######################################################################")
print("#######################################################################")
print("#######################################################################")
print(version_subversion)
print("#######################################################################")
#bintray_config = {k: v for k, v in config.get("bintray")}
#bintray_config = config.get("bintray")



#bintray_dict= {}
 # def get(self, section, option, *, raw=False, vars=None, fallback=_UNSET):
#bintray_config = config.get("bintray",option:,vars=bintray_dict)
print(config["bintray"]["user"])


print("*******************************************************************************")


#
# Push new firmware to the Bintray storage using API
#

# 'PROGNAME': 'firmware',
# 'PROG_PATH': '$BUILD_DIR\\$PROGNAME$PROGSUFFIX',

def publish_firmware(source, target, env):
    print("***************************    publish_firmware     **********************************")

    firmware_path = str(source[0])
    firmware_path=firmware_path.replace(".elf", ".bin") 

    print(firmware_path)
    firmware_name = basename(firmware_path)
    # firmware_name=firmware_name.replace(".elf", ".bin") 
    print(firmware_name)

    print("Uploading {0} to Bintray. Version: {1}".format(
        firmware_name, version_subversion))

    url = "/".join([
        "https://api.bintray.com", "content",
        config["bintray"]["user"],
        config["bintray"]["repository"] ,
        config["bintray"]["package"] , version_subversion, firmware_name
    ])
    print("*******************************************************************************")

    print(url)
    print("*******************************************************************************")


    headers = {
        "Content-type": "application/octet-stream",
        "X-Bintray-Publish": "1",
        "X-Bintray-Override": "1"
    }

    r = None
    try:
        r = requests.put(url,
                         data=open(firmware_path, "rb"),
                         headers=headers,
                         auth=(config["bintray"]["user"],
                               config["bintray"]["api_token"] ))
        r.raise_for_status()
        print (r)
    except requests.exceptions.RequestException as e:
        sys.stderr.write("Failed to submit package: %s\n" %
                         ("%s\n%s" % (r.status_code, r.text) if r else str(e)))
        env.Exit(1)

    print("The firmware has been successfuly published at Bintray.com!")


# Custom upload command and program name
#env.Replace(PROGNAME="firmware_v_%s" % version, UPLOADCMD=publish_firmware)


# projenv.Replace(PROGNAME="firmware_v_%s" % version)
# publish_firmware_v2(env)


# projenv.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", proba)
projenv.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", publish_firmware)

# projenv.AddPostAction("Build", proba)
# projenv.AddPostAction("Build", publish_firmware)
# env.AddPreAction("upload", publish_firmware)
# env.AddPostAction("Upload", publish_firmware)



#publish_firmware_v2(source, env)

# from SCons.Script import AlwaysBuild
# AlwaysBuild(env.Alias("upload", "$BUILD_DIR/${PROGNAME}", "copy /y ......"))


print("________________fin_________________")
