-- vim:sw=2:et:
local api = vim.api
local inspect = vim.inspect
local fn = vim.fn
local eval = vim.api.nvim_eval
_TEST = true
tree = require 'tree'

function t_custom_column()
  if tree.custom.custom then
    tree.custom.custom = nil
  end
  tree.custom.column('icon , filename', { directory_icon='▸', opened_icon='▾', root_icon=' ', })
  local rv = {
    column = {
      filename = { directory_icon = "▸", opened_icon = "▾", root_icon = " " },
      icon = { directory_icon = "▸", opened_icon = "▾", root_icon = " " }
    },
    option = {},
    source = {}
  }
  assert(vim.deep_equal(rv, tree.custom.custom))
end

function t_custom_option()
  if tree.custom.custom then
    tree.custom.custom = nil
  end
  tree.custom.option('_', { columns='mark:indent:icon:filename:type:size:time', })
  local rv = {
    column = {},
    option = {
      _ = { columns = "mark:indent:icon:filename:type:size:time" }
    },
    source = {}
  }
  assert(vim.deep_equal(rv, tree.custom.custom))
end

function t_custom_source()
  if tree.custom.custom then
    tree.custom.custom = nil
  end
  tree.custom.source('file', { root='Root', })
  local rv = { column = {}, option = {}, source = { file = { root = "Root" } } }
  assert(vim.deep_equal(rv, tree.custom.custom))
end
-- function t_set_custom()
--   local dest = {}
--   dest = tree._set_custom(dest, 'icon', true)
--   assert(dest, {icon=true})
--   dest = tree._set_custom(dest, {opened_icon='-', root_icon='[R]'})
--   assert(dest, {icon=true, opened_icon='-', root_icon='[R]'})
-- end
function t_init_context()
  local user_context = {
    split='vertical',
    columns='mark:indent:git:icons:filename:type',
    direction='topleft',
    resume=true,
    listed=true,
    winwidth='40'
  }
  local ctx = tree._init_context(user_context)
  local rv = {
    columns              = 'mark:indent:git:icons:filename:type',
    auto_cd              = false,
    prev_bufnr           = 1,
    root_marker          = '[in]:',
    resume               = true,
    auto_recursive_level = 0,
    sort                 = 'filename',
    listed               = true,
    new                  = false,
    ignored_files        = '.*',
    direction            = 'topleft',
    visual_end           = 0,
    winheight            = 30,
    profile              = false,
    search               = '',
    buffer_name          = 'default',
    winwidth             = '40',
    split                = 'vertical',
    visual_start         = 0,
    cursor               = 1,
    winrelative          = 'editor',
    prev_winid           = 1000,
    wincol               = 42,
    winrow               = 15,
    session_file         = '',
    show_ignored_files   = false,
    drives               = {},
    toggle               = false
  }
  local check = {
    columns = "mark:indent:git:icons:filename:type",
    custom = { column = {}, option = {}, source = { file = { root = "Root" } } },
    direction = "topleft",
    listed = true,
    resume = true,
    split = "vertical",
    wincol = 42,
    winrow = 15,
    winwidth = "40"
  }
  assert(vim.deep_equal(check, ctx))
end

function t__expand_complete()
  local rv = tree.__expand_complete('~/fool/')
  assert(rv == (vim.fn.expand('~')..'/fool/'))
  rv = tree.__expand_complete('$HOME/fool/bar/')
  print(rv)
  assert(rv == (vim.fn.expand('~')..'/fool/bar/'))
end
function t__substitute_path_separator()
  local rv = tree.__substitute_path_separator('a\\b\\c')
  if tree._is_windows then
    assert(rv == 'a/b/c')
  end
end

t__expand_complete()
t_custom_column()
t_custom_option()
t_custom_source()
-- t_set_custom()
-- t_init_context()
tree._initialize()
tree.error('test error function')
tree.warning('test error function')
tree.print_error('server dead')
assert(type(tree.string(12)) == 'string')
assert(type(tree.string('fool bar')) == 'string')
-- How to reproduce swap file error
vim.fn['tree#util#execute_path']('fool', 'bar')
