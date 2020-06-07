"=============================================================================
" FILE: tree.vim
" AUTHOR:  Shougo Matsushita <Shougo.Matsu at gmail.com>
" License: MIT license
"=============================================================================

if exists('g:loaded_tree')
  finish
endif
let g:loaded_tree = 1

command! -nargs=* -range -bar -complete=customlist,v:lua.complete
      \ Tree
      \ call tree#util#call_tree('Tree', <q-args>)
