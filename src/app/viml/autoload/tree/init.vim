"=============================================================================
" FILE: init.vim
" AUTHOR: Shougo Matsushita <Shougo.Matsu at gmail.com>
" License: MIT license
"=============================================================================

let s:project_root = fnamemodify(expand('<sfile>'), ':h:h:h')

function! tree#init#_initialize() abort
  if exists('g:tree#_channel_id')
    return
  endif

  call tree#init#_channel()

  augroup tree
    autocmd!
  augroup END

  let g:tree#_histories = []
endfunction
function! tree#init#_channel() abort
  if has('nvim') && !has('nvim-0.4.2')
    call tree#util#print_error('tree requires nvim 0.4.2+.')
    return v:true
  endif

  " TODO: temporary, ~ cant work
  if has('unix') && !has('macunix') && !has('win32unix')
    call jobstart([s:project_root . '/bin/tree-nvim', '--server', v:servername])
  elseif tree#util#is_windows()
    call jobstart([s:project_root . '\bin\tree-nvim\tree-nvim.exe', '--server', v:servername])
  else
    call jobstart([s:project_root . '/bin/tree-nvim.app/Contents/MacOS/tree-nvim', '--server', v:servername])
  endif
  sleep 100m
  echom 'jobstart success'
  return v:true
  " call tree#util#print_error(v:exception)
  " call tree#util#print_error(v:throwpoint)

endfunction
function! tree#init#_check_channel() abort
  return exists('g:tree#_channel_id')
endfunction

function! tree#init#_user_var_options() abort
  return {
        \ 'wincol': &columns / 4,
        \ 'winrow': &lines / 3,
        \ }
endfunction
function! tree#init#_user_options() abort
  return extend({
        \ 'auto_cd': v:false,
        \ 'auto_recursive_level': 0,
        \ 'buffer_name': 'default',
        \ 'columns': 'mark:indent:icon:filename:size',
        \ 'direction': '',
        \ 'ignored_files': '.*',
        \ 'listed': v:false,
        \ 'new': v:false,
        \ 'profile': v:false,
        \ 'resume': v:false,
        \ 'root_marker': '[in]: ',
        \ 'search': '',
        \ 'session_file': '',
        \ 'show_ignored_files': v:false,
        \ 'split': 'no',
        \ 'sort': 'filename',
        \ 'toggle': v:false,
        \ 'winheight': 30,
        \ 'winrelative': 'editor',
        \ 'winwidth': 90,
        \ }, tree#init#_user_var_options())
endfunction
function! s:internal_options() abort
  return {
        \ 'cursor': line('.'),
        \ 'drives': [],
        \ 'prev_bufnr': bufnr('%'),
        \ 'prev_winid': win_getid(),
        \ 'visual_start': getpos("'<")[1],
        \ 'visual_end': getpos("'>")[1],
        \ }
endfunction
function! tree#init#_context(user_context) abort
  let buffer_name = get(a:user_context, 'buffer_name', 'default')
  let context = tree#init#_user_var_options()
  let custom = tree#custom#_get()
  if has_key(custom.option, '_')
    call extend(context, custom.option['_'])
  endif
  if has_key(custom.option, buffer_name)
    call extend(context, custom.option[buffer_name])
  endif
  call extend(context, a:user_context)
  return context
endfunction

function! tree#init#action_context() abort
  let context = s:internal_options()
  return context
endfunction
