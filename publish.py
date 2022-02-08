
import sys
import os
import subprocess
import errno
import re

cmd = 'symstore.exe add /r /f build/bin/Release /s tmp /t Demo'

subprocess.call(cmd, shell=True)

