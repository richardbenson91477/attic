source /etc/environment

PATH="/local/bin:/prebuilt/jdk/bin:/usr/sbin:/usr/bin:/prebuilt/bin:/prebuilt/rescue:/prebuilt/util:/usr/games:$HOME/bin"

export CPU_N=$(cat /proc/cpuinfo | grep siblings | head -n 1 | awk '{print $3}')
export PYTHONSTARTUP="$HOME/.pythonrc"

alias make="make -j""$(($CPU_N))"
alias ls='ls --color'
alias less='less -i -RX'
alias dmesg='dmesg --color=always'
alias grep='grep --color=auto'
alias tree='tree -C'
alias open='gio open'
alias cdr='cd $(realpath "$PWD")'

#set -o vi
if [ x"$SHELL" = x"bash" ]; then
    bind "set show-all-if-ambiguous on"
    bind "set menu-complete-display-prefix on"
    fi

if [ x"$TERM" = x"xterm" ]; then
    export TERM=xterm-256color
    fi

stty -ixon

