"=============================================================================
" FILE: util.vim
" AUTHOR: Shougo Matsushita <Shougo.Matsu at gmail.com>
" License: MIT license
"=============================================================================
lua require 'tree'

function! tree#util#convert2list(expr) abort
  return type(a:expr) ==# type([]) ? a:expr : [a:expr]
endfunction

function! tree#util#execute_path(command, path) abort
  try
    execute a:command fnameescape(v:lua.__expand(a:path))
  catch /^Vim\%((\a\+)\)\=:E325/
    " Ignore swap file error
  catch
    call v:lua.tree.print_error(v:throwpoint)
    call v:lua.tree.print_error(v:exception)
  endtry
endfunction

function! tree#util#call_tree(command, args) abort
  let [paths, context] = tree#util#_parse_options_args(a:args)
  call v:lua.start(paths, context)
endfunction

function! tree#util#_parse_options_args(cmdline) abort
  return s:parse_options(a:cmdline)
endfunction
function! s:re_unquoted_match(match) abort
  " Don't match a:match if it is located in-between unescaped single or double
  " quotes
  return a:match . '\v\ze([^"' . "'" . '\\]*(\\.|"([^"\\]*\\.)*[^"\\]*"|'
        \ . "'" . '([^' . "'" . '\\]*\\.)*[^' . "'" . '\\]*' . "'" . '))*[^"'
        \ . "'" . ']*$'
endfunction
function! s:parse_options(cmdline) abort
  let args = []
  let options = {}

  " Eval
  let cmdline = (a:cmdline =~# '\\\@<!`.*\\\@<!`') ?
        \ v:lua.__eval_cmdline(a:cmdline) : a:cmdline

  for s in split(cmdline, s:re_unquoted_match('\%(\\\@<!\s\)\+'))
    let arg = substitute(s, '\\\( \)', '\1', 'g')
    let arg_key = substitute(arg, '=\zs.*$', '', '')

    let name = substitute(tr(arg_key, '-', '_'), '=$', '', '')[1:]
    if name =~# '^no_'
      let name = name[3:]
      let value = v:false
    else
      let value = (arg_key =~# '=$') ?
            \ v:lua.__remove_quote_pairs(arg[len(arg_key) :]) : v:true
    endif

    let template_opts = v:lua.user_options()
    if index(keys(template_opts), name) >= 0
      if type(template_opts[name]) == type(42)
        let options[name] = str2nr(value)
      else
        let options[name] = value
      endif
    else
      call add(args, arg)
    endif
  endfor

  return [args, options]
endfunction

function! tree#util#cd(path) abort
  if exists('*chdir')
    call chdir(a:path)
  else
    silent execute (haslocaldir() ? 'lcd' : 'cd') fnameescape(a:path)
  endif
endfunction
