#!/bin/sh
#xfwm4 -c off &
/usr/lib/x86_64-linux-gnu/xfce4/notifyd/xfce4-notifyd &
xmodmap_load
firefox_load &
xwallpaper --stretch /home/rich/Backgrounds-dl/default
pulseaudio -D
exec jwm
