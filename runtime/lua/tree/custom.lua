local M = {}

function M.get()
  if not M.custom then
    M.custom = {
      column = {},
      option = {},
      source = {},
    }
  end
  return M.custom
end

-- use name:value or dict extend dest table
local function set(dest, name_or_dict, value)
  if type(name_or_dict) == 'table' then
    dest = vim.tbl_extend('force', dest, name_or_dict)
  else
    dest[name_or_dict] = value
  end
  return dest
end

function M.column(column_name, name_or_dict, ...)
  local custom = M.get().column

  for i, key in ipairs(vim.split(column_name, '%s*,%s*')) do
    if not custom[key] then
      custom[key] = {}
    end
    custom[key] = set(custom[key], name_or_dict, ...)
  end
end

function M.option(buffer_name, name_or_dict, ...)
  local custom = M.get().option

  for i, key in ipairs(vim.split(buffer_name, '%s*,%s*')) do
    if not custom[key] then
      custom[key] = {}
    end
    custom[key] = set(custom[key], name_or_dict, ...)
  end
end

function M.source(source_name, name_or_dict, ...)
  local custom = M.get().source

  for i, key in ipairs(vim.fn.split(source_name, [[\s*,\s*]])) do
    if not custom[key] then
      custom[key] = {}
    end
    custom[key] = set(custom[key], name_or_dict, ...)
  end
end

if _TEST then
  M._set = set
end

return M
