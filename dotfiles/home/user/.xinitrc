#!/bin/sh
#xfwm4 -c off &
xwallpaper --stretch ~/Backgrounds-dl/default
xmodmap_load
~/bin/firefox_load &
xfsettingsd --replace -D --daemon
pulseaudio -D
/usr/lib/x86_64-linux-gnu/xfce4/notifyd/xfce4-notifyd &
exec jwm
