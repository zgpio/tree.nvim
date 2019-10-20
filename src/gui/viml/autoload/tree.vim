"=============================================================================
" FILE: tree.vim
" AUTHOR: Shougo Matsushita <Shougo.Matsu at gmail.com>
" License: MIT license
"=============================================================================

function! tree#initialize() abort
  return tree#init#_initialize()
endfunction

function! tree#start(paths, user_context) abort
  call tree#initialize()
  let context = tree#init#_context(a:user_context)
  let paths = a:paths
  let paths = map(paths, "fnamemodify(v:val, ':p')")
  call tree#util#rpcrequest('_tree_start', [paths, context], v:false)
  " TODO: 检查 search 是否存在
  " if context['search'] !=# ''
  "   call tree#call_action('search', [context['search']])
  " endif
endfunction

function! tree#do_action(action, ...) abort
  if &l:filetype !=# 'tree'
    return ''
  endif

  let args = tree#util#convert2list(get(a:000, 0, []))
  return printf(":\<C-u>call tree#call_action(%s, %s)\<CR>",
        \ string(a:action), string(args))
endfunction
function! tree#async_action(action, ...) abort
  if &l:filetype !=# 'tree'
    return ''
  endif

  let args = tree#util#convert2list(get(a:000, 0, []))
  return printf(":\<C-u>call tree#call_async_action(%s, %s)\<CR>",
        \ string(a:action), string(args))
endfunction
function! tree#call_action(action, ...) abort
  if &l:filetype !=# 'tree'
    return
  endif

  let context = tree#init#action_context()
  let args = tree#util#convert2list(get(a:000, 0, []))
  call tree#util#rpcrequest(
        \ '_tree_do_action', [a:action, args, context], v:false)
endfunction
function! tree#call_async_action(action, ...) abort
  if &l:filetype !=# 'tree'
    return
  endif

  let context = tree#init#action_context()
  let args = tree#util#convert2list(get(a:000, 0, []))
  call tree#util#rpcrequest(
        \ '_tree_async_action', [a:action, args, context], v:true)
endfunction

function! tree#get_candidate() abort
  if &l:filetype !=# 'tree'
    return {}
  endif

  return tree#util#rpcrequest('_tree_get_candidate', [], v:false)
endfunction
function! tree#is_directory() abort
  return get(tree#get_candidate(), 'is_directory', v:false)
endfunction
function! tree#is_opened_tree() abort
  return get(tree#get_candidate(), 'is_opened_tree', v:false)
endfunction
function! tree#get_context() abort
  if &l:filetype !=# 'tree'
    return {}
  endif

  return tree#util#rpcrequest('_tree_get_context', [], v:false)
endfunction
