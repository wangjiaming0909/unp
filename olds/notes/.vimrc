syntax on
set nu
set sw=2
set ts=2
set expandtab
set et
set smarttab
set smartindent
set langmenu=en
"set ic
set encoding=utf-8
set noswapfile
set nocompatible
set backspace=indent,eol,start
set history=1000
filetype off
set tags+=~/.vim/tags/cpp_tags
"set tags+=~/.vim/tags/protobuf_tags
"set tags+=/boost_1_72_0/boost/boost_tags
"set tags+=~/.vim/tags/ace_tags
set tags+=~/.vim/tags/sys_tags
set tags+=~/.vim/tags/event_tags
set tags+=~/.vim/tags/uv_tags
set laststatus=2
set statusline=\ %<%F[%1*%M%*%n%R%H]%=\ %y\ %0(%{&fileformat}\ %{&fileencoding}\ %c:%l/%L%)\ [%p%%]\
"set mouse=a
set lcs=tab:** "show tabs
set lcs+=trail:~ "show trailing spaces
set list
set term=screen
highlight TabLine term=underline cterm=bold ctermbg=blue ctermfg=darkgrey
highlight TabLineSel term=bold cterm=bold ctermbg=darkgrey ctermfg=white
highlight Pmenu ctermbg=lightgrey
highlight PmenuSel ctermbg=green ctermfg=yellow
"hi CursorLine cterm=bold ctermbg=darkgrey ctermfg=None guibg=#666666 guifg=lightblue
"hi cursorcolumn cterm=bold ctermbg=darkgrey ctermfg=None guibg=lightblue guifg=lightblue
hi CursorLine cterm=bold ctermbg=lightblue ctermfg=None guibg=darkgrey guifg=lightblue
hi cursorcolumn cterm=bold ctermbg=lightblue ctermfg=NONE guibg=darkgrey guifg=lightblue
hi CursorLineNr cterm=bold,underline ctermbg=blue ctermfg=yellow
set cursorline
set cursorcolumn
set fdm=marker "marker for code fold
set hlsearch
set is "incremental search

"au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe normal! g'\"" | endif

set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
Plugin 'kien/ctrlp.vim'
Plugin 'scrooloose/nerdtree'
Plugin 'taglist.vim'
Plugin 'mhinz/vim-grepper'
Plugin 'valloric/youcompleteme'
Plugin 'octol/vim-cpp-enhanced-highlight'
Plugin 'Yggdroot/indentLine'
"Plugin 'makerj/vim-pdf'
"Plugin 'clktmr/vim-gdscript3'
call vundle#end()

filetype plugin indent on
filetype on
let g:mapleader="m"
map <F3> :NERDTree<CR>
"ctrlp
let g:ctrlp_custom_ignore={
  \ 'dir':  '\v[\/]\.(clangd|vscode|git|hg|svn)$',
  \ 'file': '\v\.(so|a|o)$',
  \ 'link': 'some_bad_symbolic_links',
  \ }
"ctrlp

"taglist
map <silent> <F9> :TlistToggle<CR>
let tlist_cpp_settings = 'c++;n:namespace;v:variable;d:macro;t:typedef;' . 
                       \ 'c:class;m:member;g:enum;s:struct;u:union;f:function;' .
                       \ 'p:prototype'
let Tlist_Use_Right_Window=1
let Tlist_Exit_OnlyWindow=1
let Tlist_Process_file_Always=1
let Tlist_Show_One_File=1
set ut=500 "refresh rate
let Tlist_Use_SingleClick=1
let Tlist_WinWidth=50
"taglist
"vimgrepper
let g:grepper = {
    \'tools': ['grep'],
    \'grep': {
    \    'grepprg':    'grep -sRIEn --exclude=tags $* .',
    \    'grepprgbuf': 'grep -sHIEn -- $* $.',
    \    'grepformat': '%f:%l:%m,%f',
    \    'escape':     '\^$.*[]'
    \}}
nnoremap <leader>g :Grepper -tool grep -cword <CR>
nnoremap <leader>f :Grepper -tool grep <CR>
"vimgrepper

"highlight
let g:cpp_class_scope_highlight = 1
let g:cpp_class_decl_highlight = 1
let g:cpp_posix_standard = 1
let g:cpp_experimental_simple_template_highlight = 1
let g:cpp_experimental_template_highlight = 1
let g:cpp_concepts_highlight = 1
"highlight

map ss :w<CR>
map qq :q!<CR>
noremap <leader>1 1gt
noremap <leader>2 2gt
noremap <leader>3 3gt
noremap <leader>4 4gt
noremap <leader>5 5gt
noremap <leader>6 6gt
noremap <leader>7 7gt
noremap <leader>8 8gt
noremap <leader>9 9gt
noremap <leader>e gt
noremap <leader>q gT
noremap <leader><Left> :vertical resize -6<CR>
noremap <leader><Right> :vertical resize +6<CR>
noremap <leader><Up> :resize +6<CR>
noremap <leader><Down> :resize -6<CR>
nnoremap <C-h> <C-o>
nnoremap <C-l> <C-i>
nnoremap <F12> <C-]>
nnoremap <C-j> <C-]>:ts<CR>
nnoremap <C-k> <C-]>
nnoremap <F5> :bufdo e<CR>
nnoremap f *
nnoremap F #
nnoremap <leader>w <C-w><C-w>
nnoremap <Down> <C-e>
nnoremap <Up> <C-y>
nnoremap <leader>d <plug>(YCMHover)
nnoremap <leader>F :YcmCompleter FixIt<CR>
nnoremap D 11<C-e>
nnoremap U 11<C-y>
nnoremap <leader>m :set mouse=a<CR>
nnoremap <leader>M :set mouse=<CR>
nnoremap ;; zz
nnoremap t *N
nnoremap <leader>r :e<CR>
vnoremap // y/\V<C-R>=escape(@",'/\')<CR><CR>
let g:ycm_disable_for_files_larger_than_kb = 0
let g:ycm_max_num_identifier_candidates = 30
let g:ycm_max_num_candidates = 30
let g:ycm_collect_identifiers_from_tags_files = 1
let g:ycm_add_preview_to_completeopt = 1
let g:ycm_autoclose_preview_window_after_completion = 1
let g:ycm_autoclose_preview_window_after_insertion = 1
let g:ycm_complete_in_comments = 1

let g:indentLine_color_term = 20
let g:indentLine_color_tty_light = 7 " (default: 4)
let g:indentLine_color_dark = 1 " (default: 2)
let g:indentLine_char_list = ['¦']


autocmd BufEnter * if (winnr("$") == 1 && exists("b:NERDTree") && b:NERDTree.isTabTree()) | q | endif
autocmd BufEnter NERD_tree_* nmap  d<CR> <CR> :NERDTreeToggle <CR>
autocmd BufLeave NERD_tree_* unmap d<CR>


" NERDTress File highlighting
function! NERDTreeHighlightFile(extension, fg, bg, guifg, guibg)
  exec 'autocmd filetype nerdtree highlight ' . a:extension .' ctermbg='.  a:bg .' ctermfg='. a:fg .' guibg='. a:guibg .' guifg='. a:guifg
  exec 'autocmd filetype nerdtree syn match ' . a:extension .' #^\s\+.*'.  a:extension .'$#'
endfunction

call NERDTreeHighlightFile('jade', 'green', 'none', 'green', '#151515')
call NERDTreeHighlightFile('ini', 'yellow', 'none', 'yellow', '#151515')
call NERDTreeHighlightFile('md', 'blue', 'none', '#3366FF', '#151515')
call NERDTreeHighlightFile('yml', 'yellow', 'none', 'yellow', '#151515')
call NERDTreeHighlightFile('config', 'yellow', 'none', 'yellow', '#151515')
call NERDTreeHighlightFile('conf', 'yellow', 'none', 'yellow', '#151515')
call NERDTreeHighlightFile('json', 'yellow', 'none', 'yellow', '#151515')
call NERDTreeHighlightFile('html', 'yellow', 'none', 'yellow', '#151515')
call NERDTreeHighlightFile('styl', 'cyan', 'none', 'cyan', '#151515')
call NERDTreeHighlightFile('css', 'cyan', 'none', 'cyan', '#151515')
call NERDTreeHighlightFile('coffee', 'Red', 'none', 'red', '#151515')
call NERDTreeHighlightFile('js', 'Red', 'none', '#ffa500', '#151515')
call NERDTreeHighlightFile('php', 'Magenta', 'none', '#ff00ff', '#151515')

set background=dark


