local api = vim.api
local inspect = vim.inspect
local fn = vim.fn
local eval = vim.api.nvim_eval
require 'tree'
function t_custom_column()
  custom = nil
  custom_column('icon , filename', {
    directory_icon='▸',
    opened_icon='▾',
    root_icon=' ',
  })
  print(inspect(custom))
end
function t_custom_option()
  custom = nil
  custom_option('_', {
    columns='mark:indent:icon:filename:type:size:time',
  })
  print(inspect(custom))
end
function t_custom_source()
  custom = nil
  custom_source('file', {
    root='Root',
  })
  print(inspect(custom))
end
function t_set_custom()
  local dest = {}
  dest = set_custom(dest, 'icon', true)
  assert(dest, {icon=true})
  dest = set_custom(dest, {opened_icon='-', root_icon='[R]'})
  assert(dest, {icon=true, opened_icon='-', root_icon='[R]'})
end
function t_init_context()
  local user_context = {
    split='vertical',
    columns='mark:indent:git:icons:filename:type',
    direction='topleft',
    resume=true,
    listed=true,
    winwidth='40'
  }
  local ctx = init_context(user_context)
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
  print(inspect(ctx))
end
-- t_custom_column()
-- t_custom_option()
-- t_custom_source()
t_set_custom()
t_init_context()
initialize()
error('test error function')
warning('test error function')
