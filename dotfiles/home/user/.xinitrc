#!/bin/sh
#xfwm4 -c off &
/usr/lib/x86_64-linux-gnu/xfce4/notifyd/xfce4-notifyd &
xmodmap_load
~/bin/firefox_load &
#xwallpaper --stretch /home/rich/Backgrounds-dl/default
killall spacefm
spacefm --desktop &
pulseaudio -D
exec jwm
