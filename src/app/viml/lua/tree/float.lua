-- vim: set sw=2 sts=4 et tw=78 foldlevel=0 foldmethod=marker:
local fn = vim.fn
local api = vim.api
local cmd = vim.api.nvim_command
local function buildContent(info)
  local marker = {
    ft='',
    date='',
    size=''
  }

  local content = {}

  for k, v in pairs(info) do
    table.insert(content, string.format('%4s: %s', k, v))
  end

  return content
end

local function winPos(width, height)
  local bottom_line = fn.line('w0') + fn.winheight(0) - 1
  local curr_pos = fn.getpos('.')
  local rownr = curr_pos[2]
  local colnr = curr_pos[3]
  local columns = vim.o.columns
  -- a long wrap line
  if colnr > columns then
    colnr = colnr % columns
    rownr = rownr + colnr / columns
  end

  local vert, row, hor, col
  if rownr + height <= bottom_line then
    vert = 'N'
    row = 1
  else
    vert = 'S'
    row = 0
  end

  if colnr + width <= columns then
    hor = 'W'
    col = 0
  else
    hor = 'E'
    col = 1
  end

  return row, col, vert, hor
end

local function winSize(info, max_width, max_height)
  local width = 0
  local height = 0

  for i, line in ipairs(info) do
    local line_width = fn.strdisplaywidth(line)
    if line_width > max_width then
      width = max_width
      height = height + line_width / max_width + 1
    else
      width = fn.max({line_width, width})
      height = height + 1
    end
  end

  if height > max_height then
    height = max_height
  end
  return width, height
end

function closePopup()
  local winnr = fn.winnr('$')
  for i=1,winnr do
    if fn.getbufvar(fn.winbufnr(i), '&filetype') == 'tree-float' then
      cmd(i .. 'wincmd c')
      cmd('autocmd! TreeClosePopup * <buffer>')
      return
    end
  end
end

function Tree_display(info)
  local content = buildContent(info)
  local tree_popup_max_height
  local tree_popup_max_width
  local max_height = tree_popup_max_height or 0.6*vim.o.lines
  local max_width = tree_popup_max_width or 0.6*vim.o.columns
  max_height = fn.float2nr(max_height)
  max_width = fn.float2nr(max_width)
  local width, height = winSize(content, max_width, max_height)
  local row, col, vert, hor = winPos(width, height)

  -- for i in range(len(content)) do
  --   let line = content[i]
  -- end

  -- `width + 2`? ==> set foldcolumn=1
  local options = {
    relative='cursor',
    anchor=vert .. hor,
    row=row,
    col=col,
    width=width + 2,
    height=height,
  }
  api.nvim_open_win(fn.bufnr('%'), true, options)
  cmd('enew!')
  fn.append(0, content)

  api.nvim_exec([[
    normal gg
    nmap <silent> <buffer> q :close<CR>
    setlocal foldcolumn=1
    setlocal buftype=nofile
    setlocal bufhidden=wipe
    setlocal signcolumn=no
    setlocal filetype=tree-float
    setlocal noautoindent
    setlocal nosmartindent
    setlocal wrap
    setlocal nobuflisted
    setlocal noswapfile
    setlocal nocursorline
    setlocal nonumber
    setlocal norelativenumber
    setlocal nospell
    if has('nvim')
      setlocal winhighlight=Normal:treeFloatingNormal
      setlocal winhighlight=FoldColumn:treeFloatingNormal
    endif
    noautocmd wincmd p

    augroup TreeClosePopup
      autocmd!
      autocmd CursorMoved,CursorMovedI,InsertEnter,BufLeave,WinLeave <buffer> call v:lua.closePopup()
    augroup END
  ]], false)

end
-- print(vim.inspect(buildContent({date='2020-03-08', ft='txt'})))
-- call v:lua.Tree_display({ 'date': '2020-03-08', 'ft': 'txt' })
-- lua Tree_display({ date='2020-03-08', ft='txt', size='1024KB' })
-- autocmd CursorHold <buffer> lua Tree_display({ date='2020-03-08', ft='txt', size='1024KB' })
