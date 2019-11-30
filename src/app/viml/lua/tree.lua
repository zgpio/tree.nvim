
local buf_is_loaded = vim.api.nvim_buf_is_loaded
local call = vim.api.nvim_call_function
local cmd = vim.api.nvim_command
--- Resume tree window.
-- If the window corresponding to bufnrs is available, goto it;
-- otherwise, create a new window.
-- @param bufnrs table: trees bufnrs ordered by recently used.
-- @return nil.
function resume(bufnrs, cfg)
    print("bufnrs", vim.inspect(bufnrs))
    print("cfg", vim.inspect(cfg))

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
    print("treebufs:", vim.inspect(treebufs))

    local find = false
    -- TODO: send delete notify when -1.
    for i, bufnr in pairs(treebufs) do
        winid = call('bufwinid', {bufnr})
        if winid > 0 then
            print('goto winid', winid)
            call('win_gotoid', {winid})
            find = true
            break
        end
    end

    local bufnr = treebufs[1]
    local resize, str
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

    -- not nil => true
    if not find then
        print('split cmd:', str)
        cmd(str)
    end

    print("resize_cmd", vim.inspect(resize_cmd))
    cmd(resize_cmd)

    cmd("se nonu");
    cmd("se nornu");
    cmd("se nowrap");
    cmd("se nolist");
end

--- Drop file.
-- If the window corresponding to file is available, goto it;
-- otherwise, goto prev window and edit file.
-- @param file  string: file absolute path.
-- @return nil.
function drop(args, file)
    local arg = args[1] or 'edit'
    bufnr = call('bufnr', {file})
    winids = call('win_findbuf', {bufnr})
    -- print(vim.inspect(winids))
    if #winids == 1 then
        call('win_gotoid', {winids[1]})
    else
        prev_winnr = call('winnr', {'#'})
        prev_winid = call('win_getid', {prev_winnr})
        call('win_gotoid', {prev_winid})
        str = string.format("%s %s", arg, file)
        cmd(str)
    end
end

-- 仅仅用于处理同名文件
-- def check_overwrite(view: View, dest: Path, src: Path) -> Path:
-- dest/src: {mtime=, path=, size=}
function pre_paste(pos, dest, src)
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
    -- call('tree#util#print_message', {msg})

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
    call('tree#util#rpcrequest', {'function', {"paste", {pos, src.path, ret}}, true})
end

function pre_remove(bufnr, info)
    -- print(vim.inspect(info))
    local msg = string.format('Are you sure to remove %d files?', info.cnt)
    local choice = call('confirm', {msg, '&Yes\n&No\n&Cancel', 0})

    if choice == 1 then
        call('tree#util#rpcrequest', {'function', {"remove", {bufnr, choice}}, true})
    end
end

function buf_attach(buf)
    vim.api.nvim_buf_attach(buf, false, { on_detach = function()
        call('tree#util#rpcrequest', {'function', {"on_detach", buf}, true})
    end })
end

function rrequire(module)
    package.loaded[module] = nil
    return require(module)
end
