let s:Windows = has('win64')
let s:Unix = has('unix')
let s:Linux = has('unix') && !has('macunix') && !has('win32unix')
let s:OSX = has('macunix')

if !exists('g:colors_name')
  colorscheme desert
endif
set termguicolors
tnoremap jk <c-\><c-n>

func ExeServer()
  if s:OSX
    exe 'term ~/Documents/tree.nvim/build/bin/nvim-qt.app/Contents/MacOS/nvim-qt --server ' . v:servername
  elseif s:Linux
    exe 'term ~/tree.nvim/build/bin/nvim-qt --nofork --server ' . v:servername
  endif
  wincmd n
  call feedkeys("\<c-w>j")
  call feedkeys("G")
  call feedkeys("\<c-w>k")
  :resize +5
endf

map <c-k> :<c-u>call ExeServer()<cr>

let $root = fnamemodify(expand('<sfile>'), ':h')
set rtp+=$root/viml/
so $root/viml/plugin/tree.vim
set cmdheight=2

nnoremap <silent> <Space>e :<C-u>Tree -columns=mark:git:indent:icon:filename:size
      \ -split=vertical
      \ -direction=topleft
      \ -winwidth=40
      \ -listed
      \ `expand('%:p:h')`<CR>

nnoremap <silent> <Space>xdf :Tree -new -split=vertical -winwidth=20
            \ -columns=mark:indent:git:icon:filename:type:size -show-ignored-files
            \ -direction=topleft `expand('%:p:h')` -search=`expand('%:p')`<CR>
autocmd FileType tree call Ft()

func Ft()
  nnoremap <silent><buffer><expr> <CR> tree#async_action('drop')
  nnoremap <silent><buffer><expr> s tree#async_action('multi', [['drop', 'split']])
  nnoremap <silent><buffer><expr> cp tree#do_action('copy')
  nnoremap <silent><buffer><expr> ! tree#do_action('execute_command')
  nnoremap <silent><buffer><expr> m tree#do_action('move')
  nnoremap <silent><buffer><expr> p tree#do_action('paste')
  nnoremap <silent><buffer><expr> E tree#do_action('open', 'vsplit')
  nnoremap <silent><buffer><expr> P tree#do_action('open', 'pedit')
  nnoremap <silent><buffer><expr> O tree#async_action('open_tree_recursive')
  "nnoremap <silent><buffer><expr> K tree#do_action('new_directory')
  nnoremap <silent><buffer><expr> M tree#do_action('new_multiple_files')
  nnoremap <silent><buffer><expr> C tree#do_action('toggle_columns', 'mark:filename:type:size:time')
  nnoremap <silent><buffer><expr> S tree#do_action('toggle_sort', 'Time')
  nnoremap <silent><buffer><expr> se tree#do_action('save_session')
  nnoremap <silent><buffer><expr> sl tree#do_action('load_session')
  nnoremap <silent><buffer><expr> d tree#do_action('remove_trash')
  nnoremap <silent><buffer><expr> > tree#do_action('toggle_ignored_files')
  nnoremap <silent><buffer><expr> . tree#do_action('repeat')
  nnoremap <silent><buffer><expr> ~ tree#do_action('cd')
  nnoremap <silent><buffer><expr> \ tree#do_action('cd', getcwd())
  nnoremap <silent><buffer><expr> <Tab> tree#async_action('toggle_select') . 'j'
  nnoremap <silent><buffer><expr> * tree#do_action('toggle_select_all')
  nnoremap <silent><buffer><expr> <C-l> tree#async_action('redraw')
  nnoremap <silent><buffer><expr> <C-g> tree#do_action('print')
  "nnoremap <silent><buffer><expr> <Tab> winnr('$') != 1 ? ':<C-u>wincmd w<CR>' : ':<C-u>tree -buffer-name=temp -split=vertical<CR>'

  nnoremap <silent><buffer><expr> o tree#async_action('open_or_close_tree')
  nnoremap <silent><buffer><expr> q tree#async_action('quit')
  nnoremap <silent><buffer><expr> yy tree#async_action('yank_path')
  nnoremap <silent><buffer><expr> R tree#async_action('open_tree_recursive')
  nnoremap <silent><buffer><expr> r tree#async_action('rename')
  nnoremap <silent><buffer><expr> x tree#async_action('execute_system')
  nnoremap <silent><buffer><expr> N tree#async_action('new_file')
  nnoremap <silent><buffer><expr> h tree#async_action('cd', ['..'])
  nnoremap <silent><buffer><expr> l tree#async_action('open')

  " 默认映射不应该影响效率
  " nnoremap <silent><buffer><expr> j line('.') == line('$') ? 'gg' : 'j'
  " nnoremap <silent><buffer><expr> k line('.') == 1 ? 'G' : 'k'
endf
