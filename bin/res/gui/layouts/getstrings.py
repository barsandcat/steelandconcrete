#!/usr/bin/python
import xml.etree.ElementTree as ET


tree = ET.parse("Main.layout")

root = tree.getroot()

strings = []

for prop in root.iter():
    if prop.tag == "Property":
        if prop.get("Name", "") == "Text":
            strings.append(prop.get("Value", ""))
        

f = open("messages.h", "w")
for s in strings:
    f.write("_(\"" + s + "\");\n");
    

