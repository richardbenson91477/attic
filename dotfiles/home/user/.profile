
case "$TERM" in
    "linux"|"console")
        setfont
        ;;
    *)
        if ! grep "kitty" <<< "$TERMINFO" >& /dev/null; then
            RUN='~/bin/theme.sh $(cat ~/.config/terminal-theme)'
            eval "$RUN"
            echo eval "$RUN"
        fi
        ;;
esac
return

