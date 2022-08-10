import requests
import sys
from os.path import basename
from platformio import util

try:
    import configparser
except ImportError:
    import ConfigParser as configparser


print("####################### iniciando script python para generar credential_thinger.h ############################################")

config = configparser.RawConfigParser ()
config.read("platformio.ini")



user_entorno=config.get("usuario","entorno")
# USERNAME = smartwater_mioti

print (user_entorno)

USERNAME = config.get("thingerio_"+user_entorno, "USERNAME")

print (USERNAME)

DEVICE_ID = config.get("thingerio_"+user_entorno, "DEVICE_ID")
print (DEVICE_ID)

print (config.get("thingerio_"+user_entorno, "DEVICE_CREDENTIAL"))

DEVICE_CREDENTIAL = config.get("thingerio_"+user_entorno, "DEVICE_CREDENTIAL")
print (DEVICE_CREDENTIAL)

FILENAME_CREDENTIAL_Thinger_H = 'include/credential_thinger.h'


hf = """
#ifndef USERNAME
  #define USERNAME "{}"
#endif
#ifndef DEVICE_ID
  #define DEVICE_ID "{}"
#endif
#ifndef DEVICE_CREDENTIAL
  #define DEVICE_CREDENTIAL "{}"
#endif
""".format(USERNAME, DEVICE_ID,DEVICE_CREDENTIAL)
with open(FILENAME_CREDENTIAL_Thinger_H, 'w+') as f:
    f.write(hf)

