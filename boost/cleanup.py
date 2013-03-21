import os
import shutil

for root, dirs, file in os.walk("boost_1_53_0"):
    for name in dirs:
        #os.rmdir(os.path.join(root, name))
        if name == "doc" or name == "example" or name == "test":
            shutil.rmtree(os.path.join(root, name))
            print os.path.join(root, name)
