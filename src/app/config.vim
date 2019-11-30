set rtp+=~/Documents/tree.nvim/build/INSTALL/
set cmdheight=2

" -------------------- User Configuration --------------------
set termguicolors
nnoremap <silent> <Space>z :<C-u>Tree -columns=mark:git:indent:icon:filename:size:time
      \ -split=vertical
      \ -direction=topleft
      \ -winwidth=40
      \ -listed
      \ `expand('%:p:h')`<CR>

nnoremap <silent> <Space>xdf :Tree -new -split=vertical -winwidth=20
            \ -columns=mark:indent:git:icon:filename:type:size -show-ignored-files
            \ -direction=topleft `expand('%:p:h')` -search=`expand('%:p')`<CR>

call tree#custom#option('_', {
      \ 'root_marker': '',
      \ })

" TODO: deprecate custom#column
call tree#custom#column('filename', {
      \ 'root_marker_highlight': 'Ignore',
      \ })

autocmd FileType tree call s:set_tree()

func! s:set_tree() abort
  " Define mappings
  nnoremap <silent><buffer><expr> s tree#action('multi', [['drop', 'split'], 'quit'])
  " nnoremap <silent><buffer><expr> se tree#action('save_session')
  " nnoremap <silent><buffer><expr> sl tree#action('load_session')
  nnoremap <silent><buffer><expr> ! tree#action('execute_command')
  nnoremap <silent><buffer><expr> P tree#action('open', 'pedit')
  nnoremap <silent><buffer><expr> M tree#action('new_multiple_files')
  nnoremap <silent><buffer><expr> C tree#action('toggle_columns', 'mark:filename:type:size:time')
  nnoremap <silent><buffer><expr> S tree#action('toggle_sort', 'Time')
  nnoremap <silent><buffer><expr> d tree#action('remove_trash')
  nnoremap <silent><buffer><expr> d tree#action('remove')
  nnoremap <silent><buffer><expr> . tree#action('repeat')
  "nnoremap <silent><buffer><expr> <Tab> winnr('$') != 1 ? ':<C-u>wincmd w<CR>' : ':<C-u>tree -buffer-name=temp -split=vertical<CR>'
  "nnoremap <silent><buffer><expr> q tree#action('quit')
  nnoremap <silent><buffer><expr> cp tree#action('copy')
  nnoremap <silent><buffer><expr> m tree#action('move')
  nnoremap <silent><buffer><expr> p tree#action('paste')

  nnoremap <silent><buffer><expr> gk tree#action('goto', 'parent')
  nnoremap <silent><buffer><expr> > tree#action('toggle_ignored_files')
  nnoremap <silent><buffer><expr> * tree#action('toggle_select_all')
  nnoremap <silent><buffer><expr> s tree#action('drop', 'split')
  nnoremap <silent><buffer><expr> <CR> tree#action('drop')
  nnoremap <silent><buffer><expr> <Tab> tree#action('toggle_select') . 'j'
  nnoremap <silent><buffer><expr> <C-l> tree#action('redraw')
  nnoremap <silent><buffer><expr> <C-g> tree#action('print')
  nnoremap <silent><buffer><expr> E tree#action('open', 'vsplit')
  nnoremap <silent><buffer><expr> o tree#action('open_or_close_tree')
  nnoremap <silent><buffer><expr> R tree#action('open_tree_recursive')
  nnoremap <silent><buffer><expr> r tree#action('rename')
  nnoremap <silent><buffer><expr> x tree#action('execute_system')
  nnoremap <silent><buffer><expr> N tree#action('new_file')
  nnoremap <silent><buffer><expr> h tree#action('cd', ['..'])
  nnoremap <silent><buffer><expr> cd tree#action('cd', ['.'])
  nnoremap <silent><buffer><expr> \ tree#action('cd', getcwd())
  nnoremap <silent><buffer><expr> ~ tree#action('cd')
  nnoremap <silent><buffer><expr> l tree#action('open')
  nnoremap <silent><buffer><expr> yy tree#action('yank_path')

  " Default key mapping should not affect efficiency
  "nnoremap <silent><buffer><expr> j line('.') == line('$') ? 'gg' : 'j'
  "nnoremap <silent><buffer><expr> k line('.') == 1 ? 'G' : 'k'
endf
