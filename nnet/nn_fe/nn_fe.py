#!/usr/bin/env python3

import sys, os
import nn_draw
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk as gtk

def draw (x):
    nn_draw.main()

def compare (x):
    net = e_net.get_text()
    nn_draw.dump()
    s = "../tests/nn_map " + net + ".mnn " + net + ".mnw nn_draw.out nn_fe.out"
    os.system (s)
    fin = open ("nn_fe.out", "r")
    s = fin.readline()
    fin.close()
    e_value.set_text (s.split()[0])

def add (x):
    net = e_net.get_text()
    value = e_value.get_text()
    s = nn_draw.tostring() + value + "\n"
    fout = open (net + ".mni", "a")
    fout.write (s)
    fout.close()

def train (x):
    net = e_net.get_text()
    s = "../tests/nn_train " + net + ".mnn " + net + ".mnw"
    os.system (s)

def newnet (x):
    net = e_net.get_text()
    fout = open (net + ".mnn", "w")
    s = "64 3 1\n0.0005 0.0001 0.01 100.0 20000 1\n" +net +".mni\n" \
        +net +".mnv\n"
    fout.write (s)
    fout.close()
    try:
        os.remove (net + ".mni")
        os.remove (net + ".mnw")
        os.remove (net + ".mnv")
    except: pass

def derive (x):
    net = e_net.get_text()
    s = "./val_derive.sh " + net
    os.system (s)

def edit (x):
    net = e_net.get_text()
    s = "$EDITOR " + net + ".mnv"
    os.system (s)

def onquit (x):
    gtk.main_quit()

def main ():
    global w, b, e_value, e_net

    b = gtk.Builder()
    b.add_from_file ("nn_fe.glade")

    w = b.get_object ("window1")
    w.show()

    b.get_object ("button_paint").connect ("clicked", draw)
    b.get_object ("button_compare").connect ("clicked", compare)
    b.get_object ("button_add").connect ("clicked", add)
    b.get_object ("button_train").connect ("clicked", train)
    b.get_object ("button_quit").connect ("clicked", onquit)
    b.get_object ("button_new").connect ("clicked", newnet)
    b.get_object ("button_derive").connect ("clicked", derive)
    b.get_object ("button_edit").connect ("clicked", edit)

    e_value = b.get_object ("entry_value")
    e_net = b.get_object ("entry_network")

    e_value.set_text ("1.0")
    e_net.set_text ("test")

    w.connect ("destroy", onquit)
    gtk.main()

if __name__ == '__main__':
    sys.exit(main ())

