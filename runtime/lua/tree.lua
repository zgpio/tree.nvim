-- vim: set sw=2 sts=4 et tw=78 foldmethod=indent:
-- :luafile %
local a = vim.api
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
local custom = require('tree/custom')

-- https://gist.github.com/cwarden/1207556
function catch(what)
  return what[1]
end

function try(what)
  status, result = pcall(what[1])
  if not status then
    what[2](result)
  end
  return result
end

local M = {}

local default_etc_options = {
  winheight=30,
  winwidth=50,
  split='no', -- {"vertical", "horizontal", "no", "tab", "floating"}
  winrelative='editor',
  buffer_name='default',
  direction='',
  search='',
  new=false,
}
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
      return
    end
  end

  local bufnr = treebufs[1]
  local etc = M.etc_options[bufnr]
  local resize_cmd, str
  -- local no_split = false
  -- if cfg.split == 'no' or cfg.split == 'tab' or cfg.split == 'floating' then
  --     no_split = true
  -- end
  local vertical = ''
  local command = 'sbuffer'
  if etc.split == 'tab' then
    cmd 'tabnew'
  end
  if etc.split == 'vertical' then
    vertical = 'vertical'
    resize_cmd = string.format('vertical resize %d', etc.winwidth)
  elseif etc.split == 'horizontal' then
    resize_cmd = string.format('resize %d', etc.winheight)
  elseif etc.split == 'floating' then
    local winid = a.nvim_open_win(bufnr, true, {
      relative='editor',
      anchor='NW',
      row=0,  -- etc.winrow
      col=0,  -- etc.wincol
      width=etc.winwidth,
      height=etc.winheight,
    })
  else
    command = 'buffer'
  end

  if etc.split ~= 'floating' then
    local direction = 'topleft'
    if etc.direction == 'botright' then
      direction = 'botright'
    end
    str = string.format("silent keepalt %s %s %s %d", direction, vertical, command, bufnr)

    if not find then
      cmd(str)
    end

    cmd(resize_cmd)
  end

  cmd "se nonu"
  cmd "se nornu"
  cmd "se nolist"
  cmd "se signcolumn=no"
  a.nvim_win_set_option(winid, 'wrap', false)
end

--- Drop file.
--- If the window corresponding to file is available, goto it;
--- otherwise, goto prev window and edit file.
--@param file  string: file absolute path
--@return nil
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

--- Used to process files with the same name
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
  if choice == 0 then
    return
  elseif choice == 1 then
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

--- Confirm remove files.
--@param bufnr Number of tree buffer
--@param rmfiles List of remove files
--@return nil
function M.pre_remove(bufnr, rmfiles)
  -- print(vim.inspect(info))
  local cnt = #rmfiles
  local msg = string.format('Are you sure to remove %d files?\n', cnt)
  for _, f in ipairs(rmfiles) do
    msg = msg .. f .. '\n'
  end
  local choice = call('confirm', {msg, '&Yes\n&No\n&Cancel', 0})

  if choice == 1 then
    rpcrequest('function', {"remove", {bufnr, choice}}, true)
  end
end

function M.buf_attach(buf)
  a.nvim_buf_attach(buf, false, { on_detach = function()
    rpcrequest('function', {"on_detach", buf}, true)
    M.alive_buf_cnt = M.alive_buf_cnt - 1
    M.etc_options[buf] = nil
  end })
end

-- [first, last]
function table.slice(tbl, first, last, step)
  local sliced = {}
  for i = first or 1, last or #tbl, step or 1 do
    sliced[#sliced+1] = tbl[i]
  end
  return sliced
end
-------------------- start of util.vim --------------------
--- keymap is shared for all tree buffer
-- `:map <buffer>` to show keymap
keymap = ''
M.callback = {}
function M.keymap(lhs, ...)
  -- TODO: call directly uses lua callback
  local action_set = {
    copy=true, call=true, cd=true, drop=true, debug=true, execute_system=true,
    ['goto']=true, multi=true, move=true, new_file=true, print=true, paste=true,
    open_or_close_tree=true, open_tree_recursive=true, open=true, rename=true, redraw=true, remove=true,
    toggle_select=true, toggle_ignored_files=true, toggle_select_all=true, view=true, yank_path=true
  }
  local action_list = {...}
  local autocmd = [[augroup tree_keymap
autocmd!
autocmd FileType tree call Tree_set_keymap()
augroup END
func! Tree_set_keymap() abort
]]
  local head = [[nnoremap <silent><buffer> ]]..lhs..' '
  local str = ''
  local expr = false
  for i, action in ipairs(action_list) do
    local op, args
    if type(action) == 'table' then
      op = action[1]
      args = table.slice(action, 2)
    else
      op = action
      args = {}
    end
    for i, arg in ipairs(args) do
      if type(arg) == 'function' then
        M.callback[lhs] = arg
        expr = true
        -- NOTE: When the parameter of action is function, it should be evaluated every time
        -- print(string.format('arg: %s is function', vim.inspect(arg)))
      end
    end
    -- print(i, vim.inspect(action))
    if action_set[op] then
      if op == 'call' then
          str = str .. string.format([[:<C-U>lua tree.call(tree.callback["%s"])<CR>]], vim.fn.escape(lhs, '\\'))
      else
        if expr then
          str = str .. string.format([[:<C-u>call v:lua.call_async_action(%s, luaeval('tree.callback["%s"]()'))<CR>]], fn.string(op), vim.fn.escape(lhs, '\\'))
        else
          str = str .. string.format([[:<C-u>call v:lua.call_async_action(%s, %s)<CR>]], fn.string(op), fn.string(args))
        end
      end
    elseif vim.fn.exists(':'..op)==2 then
      str = str .. ':'..op..'<CR>'
    else
      -- TODO: Support vim action parameters
      str = str .. op
    end
  end
  keymap = keymap .. head .. str .. "\n"
  autocmd = autocmd .. keymap .. "\nendf"
  vim.api.nvim_exec(autocmd, false)
end
function M.string(expr)
  if type(expr)=='string' then
    return expr
  else
    return vim.fn.string(expr)
  end
end
function M.call_tree(command, args)
  local paths, context = __parse_options(args)
  try {
    function()
      call_async_action('redraw', {})  -- trigger exception when server dead
      start(paths, context)
    end,
    catch {
      function(error)
        print('restart tree.nvim server')
        M.channel_id = nil
        start(paths, context)
      end
    }
  }
end

--@param f function
function M.call(f)
  local ctx = M.get_candidate()
  -- a.nvim_call_function(f, {ctx})
  f(ctx)
end

function M.print_error(s)
  a.nvim_command(string.format("echohl Error | echomsg '[tree] %s' | echohl None", M.string(s)))
end

local function __re_unquoted_match(match)
  -- Don't match a:match if it is located in-between unescaped single or double quotes
  return match .. [[\v\ze([^"'\\]*(\\.|"([^"\\]*\\.)*[^"\\]*"|'([^'\\]*\\.)*[^'\\]*'))*[^"']*$]]
end
function M.convert2list(expr)
  if vim.tbl_islist(expr) then
    return expr
  else
    return {expr}
  end
end
function __parse_options(cmdline)
  local args = {}
  local options = {}
  local match = vim.fn.match

  -- Eval
  if match(cmdline, [[\\\@<!`.*\\\@<!`]]) ~= -1 then
    cmdline = __eval_cmdline(cmdline)
  end

  for _, s in ipairs(vim.fn.split(cmdline, __re_unquoted_match([[\%(\\\@<!\s\)\+]]))) do
    local arg = vim.fn.substitute(s, [[\\\( \)]], [[\1]], 'g')
    local arg_key = vim.fn.substitute(arg, [[=\zs.*$]], '', '')

    local name = vim.fn.substitute(vim.fn.tr(arg_key, '-', '_'), [[=$]], '', ''):sub(2)
    local value

    if match(name, '^no_') ~= -1 then
      name = name:sub(4)
      value = false
    else
      if match(arg_key, [[=$]]) ~= -1 then
        value = __remove_quote_pairs(arg:sub(vim.fn.len(arg_key)+1))
      else
        value = true
      end
    end

    local template_opts = user_options()
    if vim.fn.index(vim.fn.keys(template_opts), name) >= 0 then
      if type(template_opts[name]) == type(42) then
        options[name] = vim.fn.str2nr(value)
      else
        options[name] = value
      end
    else
      table.insert(args, arg)
    end
  end

  return args, options
end
function __expand(path)
  if path:find('^~') then
    path = vim.fn.fnamemodify(path, ':p')
  end
  return __substitute_path_separator(path)
end
function __remove_quote_pairs(s)
  -- remove leading/ending quote pairs
  local t = s
  if (t[1] == '"' and t[#t] == '"') or (t[1] == "'" and t[#t] == "'") then
    t = t:sub(2, #t-1)
  else
    t = vim.fn.substitute(s, [[\\\(.\)]], "\\1", 'g')
  end
  return t
end
function __substitute_path_separator(path)
  if is_windows then
    return vim.fn.substitute(path, '\\', '/', 'g')
  else
    return path
  end
end
function map_filter(func, t)
  vim.validate{func={func,'c'},t={t,'t'}}

  local rettab = {}
  for k, v in pairs(t) do
    if func(k, v) then
      rettab[k] = v
    end
  end
  return rettab
end
function __expand_complete(path)
  if path:find('^~') then
    path = vim.fn.fnamemodify(path, ':p')
  elseif vim.fn.match(path, [[^\$\h\w*]]) ~= -1 then
    path = vim.fn.substitute(path, [[^\$\h\w*]], [[\=eval(submatch(0))]], '')
  end
  return __substitute_path_separator(path)
end
function complete(arglead, cmdline, cursorpos)
  local copy = vim.fn.copy
  local _ = {}

  if arglead:find('^-') then
    -- Option names completion.
    local bool_options = vim.tbl_keys(map_filter(
      function(k, v) return type(v) == 'boolean' end, copy(user_options())))
    local bt = vim.tbl_map(function(v) return '-' .. vim.fn.tr(v, '_', '-') end, copy(bool_options))
    vim.list_extend(_, bt)
    local string_options = vim.tbl_keys(map_filter(
      function(k, v) return type(v) ~= type(true) end, copy(user_options())))
    local st = vim.tbl_map(function(v) return '-' .. vim.fn.tr(v, '_', '-') .. '=' end, copy(string_options))
    vim.list_extend(_, st)

    -- Add "-no-" option names completion.
    local nt = vim.tbl_map(function(v) return '-no-' .. vim.fn.tr(v, '_', '-') end, copy(bool_options))
    vim.list_extend(_, nt)
  else
    local al = __expand_complete(arglead)
    -- Path names completion.
    local files = vim.tbl_filter(function(v) return vim.fn.stridx(v:lower(), al:lower()) == 0 end,
      vim.tbl_map(__substitute_path_separator, vim.fn.glob(arglead .. '*', true, true)))
    files = vim.tbl_map(
      __expand_complete,
      vim.tbl_filter(function(v) return vim.fn.isdirectory(v)==1 end, files))
    if arglead:find('^~') then
      local home_pattern = '^'.. __expand_complete('~')
      files = vim.tbl_map(function(v) return vim.fn.substitute(v, home_pattern, '~/', '') end, files)
    end
    files = vim.tbl_map(function(v) return vim.fn.escape(v..'/', ' \\') end, files)
    vim.list_extend(_, files)
  end

  return vim.fn.uniq(vim.fn.sort(vim.tbl_filter(function(v) return vim.fn.stridx(v, arglead) == 0 end, _)))
end
-- Test case
-- -columns=mark:git:indent:icon:filename:size:time -winwidth=40 -listed `expand('%:p:h')`
-- -buffer-name=\`foo\` -split=vertical -direction=topleft -winwidth=40 -listed `expand('%:p:h')`
function __eval_cmdline(cmdline)
  local cl = ''
  local prev_match = 0
  local eval_pos = vim.fn.match(cmdline, [[\\\@<!`.\{-}\\\@<!`]])
  while eval_pos >= 0 do
    if eval_pos - prev_match > 0 then
      cl = cl .. cmdline:sub(prev_match+1, eval_pos)
    end
    prev_match = vim.fn.matchend(cmdline, [[\\\@<!`.\{-}\\\@<!`]], eval_pos)
    cl = cl .. vim.fn.escape(vim.fn.eval(cmdline:sub(eval_pos+2, prev_match-1)), [[\ ]])

    eval_pos = vim.fn.match(cmdline, [[\\\@<!`.\{-}\\\@<!`]], prev_match)
  end
  if prev_match >= 0 then
    cl = cl .. cmdline:sub(prev_match+1)
  end

  return cl
end
function M.new_file(args)
  print(inspect(args))
  ret = fn.input(args.prompt, args.text, args.completion)
  print(ret)
  rpcrequest('function', {"new_file", {ret, args.bufnr}}, true)
end
function M.rename(args)
  print(inspect(args))
  ret = fn.input(args.prompt, args.text, args.completion)
  if ret == "" then
    M.print_message("Cancel")
    return
  end
  rpcrequest('function', {"rename", {ret, args.bufnr}}, true)
end
function M.error(str)
  local cmd = string.format('echomsg "[tree] %s"', str)
  a.nvim_command('echohl Error')
  a.nvim_command(cmd)
  a.nvim_command('echohl None')
end
function M.warning(str)
  local cmd = string.format('echomsg "[tree] %s"', str)
  a.nvim_command('echohl WarningMsg')
  a.nvim_command(cmd)
  a.nvim_command('echohl None')
end
function M.print_message(str)
  local cmd = string.format('echo "[tree] %s"', str)
  a.nvim_command(cmd)
end

function rpcrequest(method, args, is_async)
  if not M.channel_id then
    -- TODO: temporary
    M.error("tree.channel_id doesn't exists")
    return -1
  end

  local channel_id = M.channel_id
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
-- Open a file.
function M.open(filename)
  local filename = vim.fn.fnamemodify(filename, ':p')
  local system = vim.fn.system
  local shellescape = vim.fn.shellescape
  local executable = vim.fn.executable
  local exists = vim.fn.exists
  local printf = string.format

  -- Detect desktop environment.
  if tree.windows() then
    -- For URI only.
    -- Note:
    --   # and % required to be escaped (:help cmdline-special)
    a.nvim_command(
      printf("silent execute '!start rundll32 url.dll,FileProtocolHandler %s'", vim.fn.escape(filename, '#%')))
  elseif vim.fn.has('win32unix')==1 then
    -- Cygwin.
    system(printf('cygstart %s', shellescape(filename)))
  elseif executable('xdg-open')==1 then
    -- Linux.
    system(printf('%s %s &', 'xdg-open', shellescape(filename)))
  elseif exists('$KDE_FULL_SESSION')==1 and vim.env['KDE_FULL_SESSION'] == 'true' then
    -- KDE.
    system(printf('%s %s &', 'kioclient exec', shellescape(filename)))
  elseif exists('$GNOME_DESKTOP_SESSION_ID')==1 then
    -- GNOME.
    system(printf('gnome-open %s &', shellescape(filename)))
  elseif executable('exo-open')==1 then
    -- Xfce.
    system(printf('exo-open %s &', shellescape(filename)))
  elseif tree.macos() and executable('open')==1 then
    -- Mac OS.
    system(printf('open %s &', shellescape(filename)))
  else
    -- Give up.
    M.print_error('Not supported.')
  end
end
-------------------- end of util.vim --------------------


-------------------- start of init.vim --------------------
g_servername = nil
local function init_channel()
  if fn.has('nvim-0.5') == 0 then
    print('tree requires nvim 0.5+.')
    return true
  end

  local servername = vim.v.servername
  local cmd
  -- NOTE: ~ cant expand in {cmd} arg of jobstart
  if M.linux() then
    cmd = {project_root .. '/bin/tree', servername}
  elseif M.windows() then
    local ip = '127.0.0.1'
    if not g_servername then
      local port = 6666
      while not g_servername do
        try {
          function()
            vim.fn.serverstart(ip..':'..tostring(port))
            g_servername = port
          end,
          catch {
            function(error)
              port = port + 1
            end
          }
        }
      end
    end
    cmd = {project_root .. '\\bin\\tree.exe', tostring(g_servername)}
  elseif M.macos() then
    cmd = {project_root .. '/bin/tree', servername}
  end
  -- print('bin:', bin)
  -- print('servername:', servername)
  -- print(inspect(cmd))
  fn.jobstart(cmd)
  local N = 250
  local i = 0
  while i < N and M.channel_id == nil do
    C('sleep 4m')
    i = i + 1
  end
  -- print(string.format('Wait for server %dms', i*4))
  return true
end

local function initialize()
  if M.channel_id then
    return
  end

  init_channel()
  -- NOTE: Exec VimL snippets in lua.
  a.nvim_exec([[
    augroup tree
      autocmd!
    augroup END
  ]], false)

  -- TODO: g:tree#_histories
  M.tree_histories = {}
end

-- options = core + etc
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
    columns='mark:indent:icon:filename:size',
    ignored_files='.*',
    listed=false,
    profile=false,
    resume=false,
    root_marker='[in]: ',
    session_file='',
    show_ignored_files=false,
    sort='filename',
    toggle=false,
  }, user_var_options(), default_etc_options)
end

local function internal_options()
  local s = fn.getpos("'<")[2]
  local e = fn.getpos("'>")[2]
  cmd('delmarks <')
  cmd('delmarks >')
  return {
    cursor=fn.line('.'),
    -- drives={},
    prev_bufnr=fn.bufnr('%'),
    prev_winid=fn.win_getid(),
    visual_start=s,
    visual_end=e,
  }
end
-- Transfer action context to server when perform action
-- Transfer core options when _tree_start
local function init_context(user_context)
  local buffer_name = user_context.buffer_name or 'default'
  local context = {}  -- TODO: move user_var_options to etc options
  local custom = vim.deepcopy(custom.get())
  -- NOTE: Avoid empty custom.column being converted to vector
  if vim.tbl_isempty(custom.column) then
    custom.column = nil
  end
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

-------------------- start of tree.vim --------------------
-- NOTE: The buffer creation is done by the lua side
M.alive_buf_cnt = 0
M.etc_options = {}
local count = 0
function start(paths, user_context)
  initialize()
  local context = init_context(user_context)
  local paths = fn.map(paths, "fnamemodify(v:val, ':p')")
  if #paths == 0 then
    paths = {fn.expand('%:p:h')}
  end
  if M.alive_buf_cnt < 1 or user_context.new then
    local buf = a.nvim_create_buf(false, true)
    local bufname = "Tree-" .. tostring(count)
    a.nvim_buf_set_name(buf, bufname);
    count = count + 1
    M.alive_buf_cnt = M.alive_buf_cnt + 1
    local etc_opts = vim.deepcopy(default_etc_options)
    for k, v in pairs(default_etc_options) do
      if context[k] then
        etc_opts[k] = context[k]
      end
    end
    M.etc_options[buf] = etc_opts
    context.bufnr = buf
  end
  rpcrequest('_tree_start', {paths, context}, false)
  -- TODO: 检查 search 是否存在
  -- if context['search'] !=# ''
  --   call tree#call_action('search', [context['search']])
  -- endif
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
  if type(args) ~= 'table' then
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

if _TEST then
  -- Note: we prefix it with an underscore, such that the test function and real function have
  -- different names. Otherwise an accidental call in the code to `M.FirstToUpper` would
  -- succeed in tests, but later fail unexpectedly in production
  M._set_custom = set_custom
  M._init_context = init_context
  M._initialize = initialize
  M.__expand_complete = __expand_complete
  M.custom = custom
end

tree = M
return M
