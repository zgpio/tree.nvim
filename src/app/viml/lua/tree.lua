
local call = vim.api.nvim_call_function
local cmd = vim.api.nvim_command
--- Resume tree window.
-- If the window corresponding to bufnrs is available, goto it;
-- otherwise, create a new window.
-- @param bufnrs table: trees bufnrs.
-- @return nil.
function resume(bufnrs)
    print("bufnrs", vim.inspect(bufnrs))
    for i, bufnr in pairs(bufnrs) do
        winid = call('bufwinid', {bufnr})
        if winid > 0 then
            print('goto winid', winid)
            call('win_gotoid', {winid})
            return
        end
    end
    for i, bufnr in pairs(bufnrs) do
        print('resume bufnr', bufnr)
        str = string.format("silent keepalt %s %s %s %d", "leftabove", "vertical", "sbuffer", bufnr)
        cmd(str)
    end
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
