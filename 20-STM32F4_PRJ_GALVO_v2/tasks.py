# import sublime
# import sublime_plugin

import sys, os, re
from pprint import pprint as pp 
from mdLibs import mdosd as osd 

def ind_of_substr(slist, subs):
    idx = []
    for k in range(len(slist)):
        if subs in slist(k):
            idx.append(k)
    return idx


def __init__():
    for ff in open('licverbatim', 'r+').readlines():
        f = open(os.path.join(os.path.dirname(__file__), ff.strip('\n')), 'r+')
        lines = f.readlines()
        print('file :{}: start {} end {}'.format(
            os.path.basename(f.name),
            ind_of_substr(lines, '@verbatim'), 
            ind_of_substr(lines, '@endverbatim')))

__init__()