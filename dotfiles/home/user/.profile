
case "$TERM" in
    "linux"|"console")
        setfont
        ;;
    *)
        if [ x"$TERMINFO" != x"/local/lib/kitty/terminfo" ]; then
            RUN='~/bin/theme.sh $(cat ~/.config/terminal-theme)'
            eval "$RUN"
        fi
        ;;
esac
return

