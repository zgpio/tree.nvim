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

let spath = expand('<sfile>:h') .'/dev.lua'
exe 'luafile ' . spath

autocmd FileType tree call s:set_tree()

function! CallDemo(context) abort
  echom string(a:context)
endfunction

func! s:set_tree() abort
  " Define mappings
  " nnoremap <silent><buffer><expr> <Tab> winnr('$') != 1 ? ':<C-u>wincmd w<CR>' : ':<C-u>tree -buffer-name=temp -split=vertical<CR>'

  nnoremap <silent><buffer> cD :<C-U>lua tree.call('CallDemo')<CR>
  nnoremap <silent><buffer><expr> \ tree#action('cd', getcwd())

  "nnoremap <silent><buffer><expr> j line('.') == line('$') ? 'gg' : 'j'
  "nnoremap <silent><buffer><expr> k line('.') == 1 ? 'G' : 'k'
endf
