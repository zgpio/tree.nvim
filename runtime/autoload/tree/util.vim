"=============================================================================
" FILE: util.vim
" AUTHOR: Shougo Matsushita <Shougo.Matsu at gmail.com>
" License: MIT license
"=============================================================================
lua require 'tree'

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

function! tree#util#cd(path) abort
  if exists('*chdir')
    call chdir(a:path)
  else
    silent execute (haslocaldir() ? 'lcd' : 'cd') fnameescape(a:path)
  endif
endfunction
