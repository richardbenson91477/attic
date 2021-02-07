" Vim color file
" by richardbenson91477@protonmail.com
" based originally on wombat256

hi clear

if exists("syntax_on")
    syntax reset
endif

let colors_name = "idm3-256"

hi Keyword          ctermfg=11                     cterm=none
hi Statement        ctermfg=183                     cterm=none
hi Constant         ctermfg=212                     cterm=none
hi Number           ctermfg=218                     cterm=none
hi PreProc          ctermfg=117                     cterm=none
hi Function         ctermfg=229                     cterm=none
hi Identifier       ctermfg=175                     cterm=none
hi Type             ctermfg=229                     cterm=none
hi Special          ctermfg=134                     cterm=none
hi String           ctermfg=158                     cterm=none
hi Comment          ctermfg=218                     cterm=none
hi Todo             ctermfg=235                     cterm=none
hi Directory        ctermfg=216                     cterm=none

hi Normal           ctermfg=254     ctermbg=none    cterm=none
hi Cursor           ctermfg=None    ctermbg=241     cterm=none
hi CursorLine                       ctermbg=236     cterm=none
hi Visual           ctermfg=255     ctermbg=238     cterm=none
hi Search           ctermfg=235     ctermbg=215     cterm=None
hi Folded           ctermfg=103     ctermbg=238     cterm=none
hi Title            ctermfg=250     ctermbg=232     cterm=bold
hi LineNr           ctermfg=250     ctermbg=235     cterm=none
hi VertSplit        ctermfg=238     ctermbg=238     cterm=none
hi StatusLine       ctermfg=250     ctermbg=238     cterm=none
hi StatusLineNC     ctermfg=250     ctermbg=238     cterm=none
hi MatchParen       ctermfg=250     ctermbg=67      cterm=bold
hi IncSearch        ctermfg=235     ctermbg=250     cterm=bold
hi SpecialKey       ctermfg=244     ctermbg=236     cterm=none
hi NonText          ctermfg=244     ctermbg=235     cterm=none
hi Pmenu            ctermfg=121     ctermbg=17      cterm=none
hi PmenuSel         ctermfg=121     ctermbg=24      cterm=none
hi PmenuSbar                        ctermbg=19      cterm=none
hi PmenuThumb                       ctermbg=37      cterm=none
hi WildMenu         ctermfg=232     ctermbg=35      cterm=bold
hi TabLine          ctermfg=145     ctermbg=236     cterm=none
hi TabLineFill                      ctermbg=236     cterm=none
hi TabLineSel       ctermfg=253     ctermbg=240     cterm=none

