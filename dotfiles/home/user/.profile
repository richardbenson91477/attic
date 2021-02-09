
case "$TERM" in
    "linux"|"console")
        setfont
        ;;
    "st-256color")
        ~/bin/theme.sh neon-night
        ;;
    *)
        ;;
esac
return
