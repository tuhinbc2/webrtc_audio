import os
import subprocess
import shutil
import sys
import time

import glob


#sox -e signed-integer -c 1 -b 16 -r 8000 input_bn.raw ../ab.wav


import os
for file in os.listdir("output"):
    if file.endswith(".raw"):
        #print(os.path.join("/mydir", file))
		
        ret = subprocess.call(["sox", "-e", "signed-integer", "-c", "1", "-b", "16", "-r", "8000", "output/" + str(file), "convertedwav/" + str(file.replace('raw', 'wav'))], shell=True)
	
for file in os.listdir("."):
    if file.endswith(".raw"):
        #print(os.path.join("/mydir", file))
		
        ret = subprocess.call(["sox", "-e", "signed-integer", "-c", "1", "-b", "16", "-r", "8000", str(file), "convertedwav/" + str(file.replace('raw', 'wav'))], shell=True)
	
		
		