#!/usr/bin/python

import re
from gimpfu import *

def helper_do (image, drawable, output_file_name, comment):
    gimp.progress_init("MapHelper")
    lst = pdb.gimp_path_list(image)
    print "lst[0] = " + str(lst[0])
    if lst[0] != 1:
        gimp.progress_init("Bad Path List" + str(lst[0]))
        return
    path_name = lst[1][0]
    points = pdb.gimp_path_get_points (image, path_name)
    print "points[0] = " + str(points[0])
    if points[0] != 1:
        gimp.progress_init("Bad Path Type " + str(points[0]))
        return
    coords = points[3]
    anchors = []
    for i in range(0, len(coords), 3):
        x = coords[i]
        y = coords[i+1]
        t = coords[i + 2]
        if t == 1:
            anchors += [(x,y)]
    orig_file_name = pdb.gimp_image_get_filename(image)
    print "orig_file_name = " + orig_file_name
    print "output_file_name = " + output_file_name
    if orig_file_name == None:
        gimp.progress_init("Null Orig Filename")
        return
    last_slash = orig_file_name.rfind("/") + 1
    orig_file_name = orig_file_name[last_slash:]
    f = open(output_file_name, "a")
    string = u"" + orig_file_name + " "
    for i in anchors:
        string += str(int(i[0])) + " " + str(int(i[1])) + " "
    string += comment + "\n"
    print "string = " + string
    f.write(string.encode("KOI8-R"))
    f.close()

register("python_fu_maphelper",
         "Convert the only path in the image to a line of map reference", 
         "Convert the only path in the image to a line of map reference",
         "Max Ushakov <ushakov@mccme.ru>",
         "Max Ushakov <ushakov@mccme.ru>",
         "2007",
         "<Image>/Python-Fu/Maps/Map Helper",
         "RGB*, GRAY*, INDEXED*",
         [
             (PF_FILE, "output_file_name", "Output File Name", ""),
             (PF_STRING, "comment", "Map Comment", ""),
         ],
         [],
         helper_do)

main()
