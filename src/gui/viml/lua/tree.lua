
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
