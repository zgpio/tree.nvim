
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
    if cfg.split == 'vertical' then
        resize_cmd = string.format('vertical resize %d', cfg['winwidth'])
        str = string.format("silent keepalt %s %s %s %d", "leftabove", "vertical", "sbuffer", bufnr)
    elseif cfg.split == 'horizontal' then
        resize_cmd = string.format('resize %d', cfg.winheight)
        str = string.format("silent keepalt %s %s %d", "leftabove", "sbuffer", bufnr)
    end

    -- not nil => true
    if not find then
        print('resume bufnr', bufnr)
        print('split cmd', str)
        cmd(str)
    end

    print("resize_cmd", vim.inspect(resize_cmd))
    cmd(resize_cmd)
end

--- Drop file.
-- If the window corresponding to file is available, goto it;
-- otherwise, goto prev window and edit file.
-- @param file  string: file absolute path.
-- @return nil.
function drop(file)
    bufnr = call('bufnr', {file})
    winids = call('win_findbuf', {bufnr})
    -- print(vim.inspect(winids))
    if #winids == 1 then
        call('win_gotoid', {winids[1]})
    else
        prev_winnr = call('winnr', {'#'})
        prev_winid = call('win_getid', {prev_winnr})
        call('win_gotoid', {prev_winid})
        str = string.format("edit %s", file)
        cmd(str)
    end
end

function rrequire(module)
    package.loaded[module] = nil
    return require(module)
end
