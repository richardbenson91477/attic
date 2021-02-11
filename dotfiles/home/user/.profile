
case "$TERM" in
    "linux"|"console")
        setfont
        ;;
    "xterm-256color"|"screen-256color"|"st-256color")
        ~/bin/theme.sh idm-3
        ;;
    *)
        ;;
esac
return
