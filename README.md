# Tree.nvim - File explorer powered by C++.

[![Build Status](https://circleci.com/gh/zgpio/tree.nvim.svg?style=svg)](https://circleci.com/gh/zgpio/tree.nvim)
[![Build Status](https://travis-ci.org/zgpio/tree.nvim.svg?branch=master)](https://travis-ci.org/zgpio/tree.nvim)
[![Build status](https://ci.appveyor.com/api/projects/status/v0tb04id681b49xx/branch/master?svg=true)](https://ci.appveyor.com/project/zgpio/tree-nvim/branch/master)

🎉: `QT` dependencies removed (2020-03-21)

## Requirements
- [NeoVim nightly](https://github.com/neovim/neovim/releases/tag/nightly)

## Features
- Support Linux/MacOS/Windows
- NeoVim line-based highlight (`conceal` free)
- High performance
- Defx's UI/API (minor revision)
- For more information refer to [doc](runtime/doc/tree.txt)

## Usage
![Demo](https://user-images.githubusercontent.com/19503791/86912092-f3326f00-c14e-11ea-9d98-b65563c1bd6c.png)
![Demo](https://github.com/zgpio/demo/blob/master/tree_demo.gif)

### Config
#### Step 1
```sh
mkdir tree.nvim && cd tree.nvim
# via wget or curl
sh -c "$(wget -O- https://raw.githubusercontent.com/zgpio/tree.nvim/master/install.sh)"
sh -c "$(curl -fsSL https://raw.githubusercontent.com/zgpio/tree.nvim/master/install.sh)"
```
#### Step 2
```vim
" Add tree.nvim to runtime path
set rtp+=/path/to/tree.nvim/
" Start user configuration
" ...
```
User configuration ref to [config.lua](src/app/dev.lua).
```lua
vim.o.termguicolors = true
vim.api.nvim_set_keymap('n', '<Space>z',
    ":<C-u>Tree -columns=mark:indent:git:icon:filename:size:time"..
    " -split=vertical -direction=topleft -winwidth=40 -listed `expand('%:p:h')`<CR>",
    {noremap=true, silent=true})
local custom = require 'tree/custom'
custom.option('_', {root_marker='[in]:'})
custom.column('filename', {
  root_marker_highlight='Ignore',
  max_width=60,
})
custom.column('time', {
  format="%d-%M-%Y",
})
custom.column('mark', {
  readonly_icon="X",
  selected_icon="*",
})
local tree = require('tree')
-- keymap(keys, action1, action2, ...)  action can be `vim action` or `tree action`
tree.keymap('cp', 'copy')
tree.keymap('m', 'move')
tree.keymap('p', 'paste')
tree.keymap('a', 'view')
tree.keymap('o', 'open_or_close_tree')
tree.keymap('R', 'open_tree_recursive')
tree.keymap('r', 'rename')
tree.keymap('x', 'execute_system')
tree.keymap('<CR>', 'drop')
tree.keymap('<C-l>', 'redraw')
tree.keymap('<C-g>', 'print')
tree.keymap('>', 'toggle_ignored_files')
tree.keymap('*', 'toggle_select_all')
tree.keymap('s', {'drop', 'split'}, 'quit')
tree.keymap('N', 'new_file')
tree.keymap('cd', {'cd', '.'})
tree.keymap('~', 'cd')
tree.keymap('<Tab>', 'toggle_select', 'j')  -- tree action and vim action
tree.keymap('\\', {'cd', vim.fn.getcwd})
tree.keymap('cD', {'call', function(context) print(vim.inspect(context)) end})
tree.keymap('l', 'open')
tree.keymap('yy', 'yank_path')
tree.keymap('D', 'debug')
tree.keymap('d', 'remove')
tree.keymap('E', {'open', 'vsplit'})
tree.keymap('h', {'cd', '..'})
tree.keymap('gk', {'goto', 'parent'})
```

## Build
### macos
```sh
brew install boost
git clone https://github.com/zgpio/tree.nvim

cmake -DCMAKE_INSTALL_PREFIX=./INSTALL -DBoost_USE_STATIC_LIBS=ON -DCMAKE_BUILD_TYPE=Release -S tree.nvim/ -B tree.nvim/build
make -C tree.nvim/build/ install
```
### ubuntu1804
```sh
# cmake 3.10  boost 1.65
sudo apt install cmake libboost-filesystem-dev
git clone https://github.com/zgpio/tree.nvim.git
cd tree.nvim
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=./INSTALL -DBoost_USE_STATIC_LIBS=ON -DCMAKE_BUILD_TYPE=Release  ..
make install
```
