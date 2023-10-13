import shutil
import os

APP_CONFIG = "src/AppConfig.h"
LIB_AUDIO_DETECTION = "new-enzus-project-1_inferencing"
LIB_IMAGE_DETECTION = "water-vision3_inferencing"
method = "IMAGE_DETECTION"

print("============================================================================")

cfg_file = open(APP_CONFIG, 'r')
while True:
    line = cfg_file.readline()
    if not line:
        break
    if "DETECTION_METHOD" in line:
        if "AUDIO_DETECTION" in line:
            method = "AUDIO_DETECTION"
        break

print("Using %s" % (method))

lib_src = ""
lib_dst = ""
lib_rmv = ""
if method == "AUDIO_DETECTION":
    lib_src = os.path.join("libdetection", LIB_AUDIO_DETECTION)
    lib_dst = os.path.join("lib", LIB_AUDIO_DETECTION)
    lib_rmv = os.path.join("lib", LIB_IMAGE_DETECTION)
elif method == "IMAGE_DETECTION":
    lib_src = os.path.join("libdetection", LIB_IMAGE_DETECTION)
    lib_dst = os.path.join("lib", LIB_IMAGE_DETECTION)
    lib_rmv = os.path.join("lib", LIB_AUDIO_DETECTION)

if os.path.isdir(lib_rmv):
    print("Remove %s" % (lib_rmv))
    shutil.rmtree(lib_rmv)

if os.path.isdir(lib_dst):
    print("Lib detection %s is ok" % (lib_dst))
else:
    if os.path.isdir(lib_src):
        print("Copy detection lib to %s" % (lib_dst))
        shutil.copytree(lib_src, lib_dst)
    else:
        print("Pls copy detection lib to %s" % (lib_src))

print("============================================================================")
