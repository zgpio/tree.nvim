# Tree.nvim - File explorer powered by C++.

Experimental, maybe has breaking changes and bugs.

[![Build Status](https://circleci.com/gh/zgpio/tree.nvim.svg?style=svg)](https://circleci.com/gh/zgpio/tree.nvim)
[![Build Status](https://ci.appveyor.com/api/projects/status/o1wpqwdyclyaffa4/branch/master?svg=true)](https://ci.appveyor.com/project/zgpio/tree-nvim/branch/master)
[![Build Status](https://travis-ci.org/zgpio/tree.nvim.svg?branch=master)](https://travis-ci.org/zgpio/tree.nvim)
[![Downloads](https://img.shields.io/github/downloads/zgpio/tree.nvim/total)](https://github.com/zgpio/tree.nvim/releases) [![Join the chat at https://gitter.im/tree-nvim/community](https://badges.gitter.im/tree-nvim/community.svg)](https://gitter.im/tree-nvim/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Requirements
`>= neovim 0.4.2`

## Features
- NeoVim line-based highlight (Not dependent on conceal).
- NeoVim support only.

## Using
![Demo](https://user-images.githubusercontent.com/19503791/67760636-a0ab9980-fa7c-11e9-844f-c5acea41d7f1.png)
### Config
```vim
set termguicolors
nnoremap <silent> <Space>z :<C-u>Tree -columns=mark:git:indent:icon:filename:size
      \ -split=vertical
      \ -direction=topleft
      \ -winwidth=40
      \ -listed
      \ `expand('%:p:h')`<CR>

call tree#custom#option('_', {
      \ 'root_marker': '',
      \ })

autocmd FileType tree call s:set_tree()
func! s:set_tree() abort
  nnoremap <silent><buffer><expr> <C-l> tree#action('redraw')
  nnoremap <silent><buffer><expr> <C-g> tree#action('print')
  nnoremap <silent><buffer><expr> <Tab> tree#action('toggle_select') . 'j'
  nnoremap <silent><buffer><expr> <CR> tree#action('drop')
  nnoremap <silent><buffer><expr> E tree#action('open', 'vsplit')
  nnoremap <silent><buffer><expr> o tree#action('open_or_close_tree')
  nnoremap <silent><buffer><expr> q tree#action('quit')
  nnoremap <silent><buffer><expr> R tree#action('open_tree_recursive')
  nnoremap <silent><buffer><expr> r tree#action('rename')
  nnoremap <silent><buffer><expr> x tree#action('execute_system')
  nnoremap <silent><buffer><expr> N tree#action('new_file')
  nnoremap <silent><buffer><expr> h tree#action('cd', ['..'])
  nnoremap <silent><buffer><expr> l tree#action('open')
  nnoremap <silent><buffer><expr> yy tree#action('yank_path')

  " nnoremap <silent><buffer><expr> j line('.') == line('$') ? 'gg' : 'j'
  " nnoremap <silent><buffer><expr> k line('.') == 1 ? 'G' : 'k'
endf

```
