#!/usr/bin/python
import os
import sys
import subprocess

output = "src/scm_revision.h";
print "Finding revision " + os.getcwd() + "; output: " + output;

revnum = subprocess.Popen(["hg", "log", "-l", "1", "--template", "{node}"], stdout=subprocess.PIPE).communicate()[0];

if os.path.exists(output):
    file = open(output, "r");
    contents = file.readline();
    file.close();    
    oldrevnum = contents.split('"')[1];
    if revnum == oldrevnum:
        print "Same revsion " + oldrevnum + " as last time: not updating."
        sys.exit(0);

file = open(output, "w");
file.write('#define SCM_REVISION "' + revnum + '"');
file.close();
print "Wrote revision " + revnum + " to " + output;

        
