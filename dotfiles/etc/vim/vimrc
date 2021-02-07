set nocompatible
set viminfo=""
set number
set confirm
set expandtab
set tabstop=4
set shiftwidth=4
set autoindent
set hlsearch
set ignorecase
set smartcase
set mouse=a
set scrolloff=4
set tabpagemax=16
set cmdheight=1
set relativenumber

let g:rainbow_active = 1
let g:netrw_altv = 1
let g:netrw_winsize = 25

filetype indent plugin on

colorscheme idm2-256
syntax on

let mapleader=","

" quitting
nnoremap <leader>qq :qa<cr>
nnoremap <leader>qs :w!<bar>:qa<cr>
nnoremap <C-c><C-q> :qa<cr>
nnoremap <C-c><C-s> :w!<bar>q<cr>
nnoremap <C-c><C-x> :qa!<cr>

" misc
nnoremap <C-g> :noh<cr>
nnoremap <leader>b <C-o>
inoremap ;; <esc>

" clipboard (system)
vnoremap <leader>y "+y
nnoremap <leader>p "+p
nnoremap <leader>P "+P

" display 
nnoremap <leader>dd :Lexplore<cr>
nnoremap <leader>dt :Tagbar<cr>

" files
nnoremap <leader>fi :file<cr>
nnoremap <leader>fq :q<cr>
nnoremap <leader>fs :w<cr>
nnoremap <leader>fl :edit<space>
nnoremap <leader>fn :enew<cr>
nnoremap <leader>fr :read<space>
nnoremap <leader>fj :next<cr>
nnoremap <leader>fk :prev<cr>
nnoremap <leader>fc :cd %:p:h<cr>

" ctags
nnoremap <leader>cl :exec ("set tags=".getcwd()."/tags")<cr>
nnoremap <leader>cr :exec ("! build_these_tags ".expand("<tags>"))<cr>
nnoremap <leader>cg <C-]>
nnoremap <leader>cG :vsplit<bar>exec("tag ".expand("<cword>"))<cr>
nnoremap <leader>cb <C-T>

" searching
nnoremap <leader>sw :exec ("grep -r ".expand("<cword>")." .")<cr>
nnoremap <leader>sW :exec ("grep -r ".expand("<cword>")." %:p:h/")<cr>
nnoremap <leader>ss :exec ("grep -r ".input("Search:")." .")<cr>
nnoremap <leader>sS :exec ("grep -r ".input("Search:")." %:p:h/")<cr>
nnoremap <leader>sd :exec ("grep -r ".input("Search: ")." ".input("Where: "))<cr>

" errors (and search results)
nnoremap <leader>eo :copen<cr>
nnoremap <leader>ef :cfirst<cr>
nnoremap <leader>en :cnext<cr>
nnoremap <leader>ep :cprev<cr>

" windowing
nnoremap <leader>wn :vsplit<bar>:enew<cr>
nnoremap <leader>wN <c-w>n
nnoremap <leader>wq <c-w>q
nnoremap <leader>w1 <c-w>o
nnoremap <leader>ws <c-w>s
nnoremap <leader>wv <c-w>v
nnoremap <leader>wr <c-w>r
nnoremap <leader>ww <c-w>w
nnoremap <leader>wh <c-w>h
nnoremap <leader>wl <c-w>l
nnoremap <leader>wj <c-w>j
nnoremap <leader>wk <c-w>k
nnoremap <A-h> <C-w>h
nnoremap <A-l> <C-w>l
nnoremap <A-j> <C-w>j
nnoremap <A-k> <C-w>k

" resizing windows
nnoremap <leader>rm <c-w><bar>
nnoremap <leader>rM <c-w>_
nnoremap <leader>re <c-w>=
nnoremap <leader>rh <c-w><
nnoremap <leader>rl <c-w>>
nnoremap <leader>rj <c-w>+
nnoremap <leader>rk <c-w>-
nnoremap <leader>rH <c-w>20<
nnoremap <leader>rL <c-w>20>
nnoremap <leader>rJ <c-w>10+
nnoremap <leader>rK <c-w>10-

" tabbing
nnoremap <leader>to :tabnew 
nnoremap <leader>tq :tabclose<cr>
nnoremap <leader>t1 :tabonly<cr>
nnoremap <leader>tl :tabnext<cr>
nnoremap <leader>th :tabprev<cr>

" making
nnoremap <leader>mm :wall<bar>make!<cr>
nnoremap <leader>mB :wall<bar>make! -B<cr>
nnoremap <leader>mt :wall<bar>make! test<cr>
nnoremap <leader>mu :wall<bar>make! upload<cr>
nnoremap <leader>mp :wall<bar>make! program<cr>
nnoremap <leader>mc :make! clean<cr>
nnoremap <leader>mb :w<bar>make! %:t:r<cr>
nnoremap <leader>mo :w<bar>make! %:t:r.o<cr>

" going !exec
nnoremap <leader>gh :w<bar>!./%<cr>
nnoremap <leader>gb :!./%:t:r<cr>
nnoremap <leader>gs :w<bar>!guile %<cr>
nnoremap <leader>gg :w<bar>!go run .<cr>
nnoremap <leader>gt :set splitright<bar>:vsplit<bar>:set nosplitright<bar>terminal $SHELL -l<cr>
nnoremap <leader>gT :set splitbelow<bar>:split<bar>:set nosplitbelow<bar>terminal $SHELL -l<cr>

" version control
nnoremap <leader>vc :w<bar>!git commit -a<cr>
nnoremap <leader>vp :!git push<cr>
nnoremap <leader>vd :!git --no-pager diff --color<cr>

" terminal
tnoremap <Esc> <C-\><C-n>
tnoremap <A-h> <C-\><C-N><C-w>h
tnoremap <A-l> <C-\><C-N><C-w>l
tnoremap <A-j> <C-\><C-N><C-w>j
tnoremap <A-k> <C-\><C-N><C-w>k

" per-directory ctags
if filereadable("./tags")
    if input("Load ./tags? ") == "y"
        exec "set tags=".getcwd()."/tags"
    endif
endif

" per-directory configs
if filereadable("./vimrc.ext")
    if input("Load ./vimrc.ext? ") == "y"
        source ./vimrc.ext
    endif
endif

" load APL files as Julia for nifty \latex<tab> character insertion
augroup apl_ft
    au!
    autocmd BufNewFile,BufRead *.apl set filetype=julia
augroup END

