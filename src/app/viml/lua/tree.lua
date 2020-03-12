-- vim: set sw=2 sts=4 et tw=78 foldlevel=0 foldmethod=indent:
-- :luafile %
local api = vim.api
local inspect = vim.inspect
local fn = vim.fn
local eval = vim.api.nvim_eval
local C = vim.api.nvim_command
local cmd = vim.api.nvim_command
local buf_is_loaded = vim.api.nvim_buf_is_loaded
local call = vim.api.nvim_call_function

local is_windows = fn.has('win32') == 1 or fn.has('win64') == 1
local is_macos = not is_windows and fn.has('win32unix') == 0 and fn.has('macunix') == 1
local is_linux = fn.has('unix') == 1 and fn.has('macunix') == 0 and fn.has('win32unix') == 0
local info = debug.getinfo(1, "S")
local sfile = info.source:sub(2) -- remove @
local project_root = fn.fnamemodify(sfile, ':h:h')

local M = {}

--- Resume tree window.
-- If the window corresponding to bufnrs is available, goto it;
-- otherwise, create a new window.
-- @param bufnrs table: trees bufnrs ordered by recently used.
-- @return nil.
function M.resume(bufnrs, cfg)
  if bufnrs == nil then
    return
  end
  if type(bufnrs) == 'number' then
    bufnrs = {bufnrs}
  end

  -- check bufnrs
  local deadbufs = {}
  local treebufs = {}
  for i, bufnr in pairs(bufnrs) do
    loaded = buf_is_loaded(bufnr)
    if loaded then
      table.insert(treebufs, bufnr)
    else
      table.insert(deadbufs, bufnr)
    end
  end
  -- print("treebufs:", vim.inspect(treebufs))

  local find = false
  -- TODO: send delete notify when -1.
  for i, bufnr in pairs(treebufs) do
    local winid = call('bufwinid', {bufnr})
    if winid > 0 then
      print('goto winid', winid)
      call('win_gotoid', {winid})
      find = true
      break
    end
  end

  local bufnr = treebufs[1]
  local resize_cmd, str
  -- local no_split = false
  -- if cfg.split == 'no' or cfg.split == 'tab' or cfg.split == 'floating' then
  --     no_split = true
  -- end
  local vertical = ''
  local command = 'sbuffer'
  if cfg.split == 'tab' then
    cmd('tabnew')
  end
  if cfg.split == 'vertical' then
    vertical = 'vertical'
    resize_cmd = string.format('vertical resize %d', cfg['winwidth'])
  elseif cfg.split == 'horizontal' then
    resize_cmd = string.format('resize %d', cfg.winheight)
  else
    command = 'buffer'
  end

  local direction = 'topleft'
  if cfg.direction == 'botright' then
    direction = 'botright'
  end
  str = string.format("silent keepalt %s %s %s %d", direction, vertical, command, bufnr)

  if not find then
    cmd(str)
  end

  cmd(resize_cmd)

  cmd("se nonu");
  cmd("se nornu");
  cmd("se nowrap");
  cmd("se nolist");
  cmd("se signcolumn=no");
end

--- Drop file.
-- If the window corresponding to file is available, goto it;
-- otherwise, goto prev window and edit file.
-- @param file  string: file absolute path.
-- @return nil.
function M.drop(args, file)
  local arg = args[1] or 'edit'
  local bufnr = call('bufnr', {file})
  local winids = call('win_findbuf', {bufnr})
  -- print(vim.inspect(winids))
  if #winids == 1 then
    call('win_gotoid', {winids[1]})
  else
    local prev_winnr = call('winnr', {'#'})
    local prev_winid = call('win_getid', {prev_winnr})
    call('win_gotoid', {prev_winid})
    local str = string.format("%s %s", arg, file)
    cmd(str)
  end
end

-- 仅仅用于处理同名文件
-- def check_overwrite(view: View, dest: Path, src: Path) -> Path:
-- dest/src: {mtime=, path=, size=}
function M.pre_paste(pos, dest, src)
  -- print(vim.inspect(dest))
  local d_mtime = dest.mtime
  local s_mtime = src.mtime

  local slocaltime = os.date("%Y-%m-%d %H:%M:%S", s_mtime)
  local dlocaltime = os.date("%Y-%m-%d %H:%M:%S", d_mtime)
  -- time.strftime("%c", time.localtime(s_mtime))
  local msg1 = string.format(' src: %s %d bytes\n', src.path, src.size)
  local msg2 = string.format('      %s\n', slocaltime)
  local msg3 = string.format('dest: %s %d bytes\n', dest.path, dest.size)
  local msg4 = string.format('      %s\n', dlocaltime)
  local msg = msg1..msg2..msg3..msg4
  -- print_message(msg)

  local msg = msg..string.format('%s already exists.  Overwrite?', dest.path)
  local choice = call('confirm', {msg, '&Force\n&No\n&Rename\n&Time\n&Underbar', 0})
  local ret = ''
  if choice == 1 then
    ret = dest.path
  elseif choice == 2 then
    ret = ''
  elseif choice == 3 then
    -- ('dir' if src.is_dir() else 'file')
    local msg = string.format('%s -> ', src.path)
    ret = call('input', {msg, dest.path, 'file'})
  elseif choice == 4 and d_mtime < s_mtime then
    ret = src.path
  elseif choice == 5 then
    ret = dest.path .. '_'
  end

  -- TODO: notify ret to server --
  rpcrequest('function', {"paste", {pos, src.path, ret}}, true)
end

function M.pre_remove(bufnr, info)
  -- print(vim.inspect(info))
  local msg = string.format('Are you sure to remove %d files?', info.cnt)
  local choice = call('confirm', {msg, '&Yes\n&No\n&Cancel', 0})

  if choice == 1 then
    rpcrequest('function', {"remove", {bufnr, choice}}, true)
  end
end

function M.buf_attach(buf)
  vim.api.nvim_buf_attach(buf, false, { on_detach = function()
    rpcrequest('function', {"on_detach", buf}, true)
  end })
end

-------------------- start of util.vim --------------------
function M.error(str)
  local cmd = string.format('echomsg "[tree] %s"', str)
  vim.api.nvim_command('echohl Error')
  vim.api.nvim_command(cmd)
  vim.api.nvim_command('echohl None')
end
function M.warning(str)
  local cmd = string.format('echomsg "[tree] %s"', str)
  vim.api.nvim_command('echohl WarningMsg')
  vim.api.nvim_command(cmd)
  vim.api.nvim_command('echohl None')
end
function M.print_message(str)
  local cmd = string.format('echo "[tree] %s"', str)
  vim.api.nvim_command(cmd)
end

local function check_channel()
  return fn.exists('g:tree#_channel_id')
end
function rpcrequest(method, args, is_async)
  if check_channel() == 0 then
    -- TODO: temporary
    M.error("g:tree#_channel_id doesn't exists")
    return -1
  end

  local channel_id = vim.g['tree#_channel_id']
  if is_async then
    return vim.rpcnotify(channel_id, method, args)
  else
    return vim.rpcrequest(channel_id, method, args)
  end
end

function M.linux()
  return is_linux
end
function M.windows()
  return is_windows
end
function M.macos()
  return is_macos
end
-------------------- end of util.vim --------------------


-------------------- start of init.vim --------------------
-- cant work in lua script
-- print(vim.fn.expand('<sfile>'))
local function init_channel()
  if fn.has('nvim-0.5') == 0 then
    print('tree requires nvim 0.5+.')
    return true
  end

  local servername = vim.v.servername
  local cmd
  -- NOTE: ~ cant expand in {cmd} arg of jobstart
  if M.linux() then
    cmd = {project_root .. '/bin/tree-nvim', '--nofork', '--server', servername}
  elseif M.windows() then
    cmd = {project_root .. '\\bin\\tree-nvim.exe', '--server', servername}
  elseif M.macos() then
    cmd = {project_root .. '/bin/tree-nvim.app/Contents/MacOS/tree-nvim', '--server', servername}
  end
  -- print('bin:', bin)
  -- print('servername:', servername)
  -- print(inspect(cmd))
  fn.jobstart(cmd)
  local N = 15
  local i = 0
  while i < N and fn.exists('g:tree#_channel_id') == 0 do
    C('sleep 10m')
    i = i + 1
  end
  -- print(string.format('Wait for server %dms', i*10))
  return true
end

local function initialize()
  if fn.exists('g:tree#_channel_id') == 1 then
    return
  end

  init_channel()
  -- NOTE: Exec VimL snippets in lua.
  vim.api.nvim_exec([[
    augroup tree
      autocmd!
    augroup END
  ]], false)

  -- TODO: g:tree#_histories
  M.tree_histories = {}
end

local function user_var_options()
  return {
    wincol=math.modf(vim.o.columns/4),
    winrow=math.modf(vim.o.lines/3)
  }
end
function user_options()
  return vim.tbl_extend('force', {
    auto_cd=false,
    auto_recursive_level=0,
    buffer_name='default',
    columns='mark:indent:icon:filename:size',
    direction='',
    ignored_files='.*',
    listed=false,
    new=false,
    profile=false,
    resume=false,
    root_marker='[in]: ',
    search='',
    session_file='',
    show_ignored_files=false,
    split='no',
    sort='filename',
    toggle=false,
    winheight=30,
    winrelative='editor',
    winwidth=90,
  }, user_var_options())
end

local function custom_get()
  if not M.custom then
    M.custom = {
      column = {},
      option = {},
      source = {},
    }
  end
  return M.custom
end

local function internal_options()
  return {
    cursor=fn.line('.'),
    drives={},
    prev_bufnr=fn.bufnr('%'),
    prev_winid=fn.win_getid(),
    visual_start=fn.getpos("'<")[2],
    visual_end=fn.getpos("'>")[2],
  }
end
-- 一些设置没有必要传输, action_ctx/setting_ctx
local function init_context(user_context)
  local buffer_name = user_context.buffer_name or 'default'
  local context = user_var_options()
  local custom = custom_get()
  if custom.option._ then
    context = vim.tbl_extend('force', context, custom.option._)
    custom.option._ = nil
  end
  if custom.option.buffer_name then
    context = vim.tbl_extend('force', context, custom.option.buffer_name)
  end
  context = vim.tbl_extend('force', context, user_context)
  -- TODO: support custom#column
  context.custom = custom
  return context
end

local function action_context()
  local context = internal_options()
  return context
end

-------------------- end of init.vim --------------------

-------------------- start of custom.vim --------------------
-- 用name:value或dict扩展dest table
local function set_custom(dest, name_or_dict, value)
  if type(name_or_dict) == 'table' then
    dest = vim.tbl_extend('force', dest, name_or_dict)
  else
    dest[name_or_dict] = value
  end
  return dest
end

function M.custom_column(column_name, name_or_dict, ...)
  local custom = custom_get().column

  for i, key in ipairs(vim.split(column_name, '%s*,%s*')) do
    if not custom[key] then
      custom[key] = {}
    end
    custom[key] = set_custom(custom[key], name_or_dict, ...)
  end
end

function M.custom_option(buffer_name, name_or_dict, ...)
  local custom = custom_get().option

  for i, key in ipairs(vim.split(buffer_name, '%s*,%s*')) do
    if not custom[key] then
      custom[key] = {}
    end
    custom[key] = set_custom(custom[key], name_or_dict, ...)
  end
end

function M.custom_source(source_name, name_or_dict, ...)
  local custom = custom_get().source

  for i, key in ipairs(fn.split(source_name, [[\s*,\s*]])) do
    if not custom[key] then
      custom[key] = {}
    end
    custom[key] = set_custom(custom[key], name_or_dict, ...)
  end
end
-------------------- end of custom.vim --------------------

-------------------- start of tree.vim --------------------
function start(paths, user_context)
  initialize()
  local context = init_context(user_context)
  local paths = fn.map(paths, "fnamemodify(v:val, ':p')")
  if #paths == 0 then
    paths = {fn.expand('%:p:h')}
  end
  rpcrequest('_tree_start', {paths, context}, false)
  -- TODO: 检查 search 是否存在
  -- if context['search'] !=# ''
  --   call tree#call_action('search', [context['search']])
  -- endif
end
function action(action, ...)
  if vim.bo.filetype ~= 'tree' then
    return ''
  end
  local args = ...
  if type(args) ~= type({}) then
    args = {...}
  end
  return api.nvim_eval(string.format([[":\<C-u>call v:lua.call_async_action(%s, %s)\<CR>"]],
         fn.string(action), fn.string(args)))
end

function M.call_action(action, ...)
  if vim.bo.filetype ~= 'tree' then
    return
  end

  local context = action_context()
  local args = ...
  if type(args) ~= type({}) then
    args = {...}
  end
  rpcrequest('_tree_do_action', {action, args, context}, false)
end
function call_async_action(action, ...)
  if vim.bo.filetype ~= 'tree' then
    return
  end

  local context = action_context()
  local args = ...
  if type(args) ~= type({}) then
    args = {...}
  end
  rpcrequest('_tree_async_action', {action, args, context}, true)
end

function M.get_candidate()
  if vim.bo.filetype ~= 'tree' then
    return {}
  end

  local context = internal_options()
  return rpcrequest('_tree_get_candidate', {context}, false)
end
function M.is_directory()
  return fn.get(M.get_candidate(), 'is_directory', false)
end
function M.is_opened_tree()
  return fn.get(M.get_candidate(), 'is_opened_tree', false)
end

function M.get_context()
  if vim.bo.filetype ~= 'tree' then
    return {}
  end

  return rpcrequest('_tree_get_context', {}, false)
end
-------------------- end of tree.vim --------------------
function M.refactor(old)
  local C = vim.api.nvim_command
  local msg = 'Rename to: '
  local new = fn.input(msg, '', 'file')
  local cmd = string.format(':%%s/\\<%s\\>/%s/gIc', old, new)
  print(cmd)
  C(cmd)
end

function M.rrequire(module)
  package.loaded[module] = nil
  return require(module)
end

return M
