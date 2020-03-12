lua require 'tree'

function! tree#action(action, ...) abort
  if &l:filetype !=# 'tree'
    return ''
  endif

  let args = tree#util#convert2list(get(a:000, 0, []))
  return printf(":\<C-u>call v:lua.call_async_action(%s, %s)\<CR>",
        \ string(a:action), string(args))
endfunction
