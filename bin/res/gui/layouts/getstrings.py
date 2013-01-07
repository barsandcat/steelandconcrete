#!/usr/bin/python
import xml.etree.ElementTree as ET
import glob



def ParseLayout(fileName):
    tree = ET.parse(fileName)

    root = tree.getroot()

    for prop in root.iter():
        if prop.tag == "Property":
            if prop.get("Name", "") == "Text":
                strings.append(prop.get("Value", ""))


strings = []


for files in glob.glob("*.layout"):
    print files
    ParseLayout(files)

f = open("messages.h", "w")
for s in strings:
    f.write("_(\"" + s + "\");\n");
