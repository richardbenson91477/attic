#!/bin/sh
#xfwm4 -c off &
/usr/lib/x86_64-linux-gnu/xfce4/notifyd/xfce4-notifyd &
xmodmap_load
~/bin/firefox_load &
killall spacefm
spacefm --desktop &
pulseaudio -D
exec jwm
