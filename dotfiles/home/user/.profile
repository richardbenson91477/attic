
case "$TERM" in
    "linux"|"console")
        setfont
        ;;
    "xterm-256color"|"screen-256color"|"st-256color")#|"xterm-kitty")
        ~/bin/theme.sh $(cat ~/.config/terminal-theme)
        ;;
    "xterm-kitty")
        export TERM="xterm-256color"
        ;;
    *)
        ;;
esac
return
