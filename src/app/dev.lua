local custom = require 'tree/custom'
custom.option('_', {root_marker='[in]:', })
custom.column('filename', {
  root_marker_highlight='Ignore',
  max_width=60,
})
custom.column('time', {
  format="%d-%M-%Y",
})
custom.column('mark', {
  readonly_icon="X",
  -- selected_icon="*",
})
local tree = require('tree')
-- User interface design
-- keymap(keys, action1, action2, ...)  action can be `vim action` or `tree action`
tree.keymap('cp', 'copy')
tree.keymap('m', 'move')
tree.keymap('p', 'paste')
tree.keymap('a', 'view')
tree.keymap('o', 'open_or_close_tree')
tree.keymap('R', 'open_tree_recursive')
tree.keymap('r', 'rename')
-- tree.keymap('!', 'execute_command')
-- tree.keymap('se', 'save_session')
-- tree.keymap('sl', 'load_session')
-- tree.keymap('P', {'open', 'pedit'})
-- tree.keymap('M', 'new_multiple_files')
-- tree.keymap('C', {'toggle_columns', 'mark:filename:type:size:time'})
-- tree.keymap('S', {'toggle_sort', 'Time'})
-- tree.keymap('d', 'remove_trash')
-- tree.keymap('.', 'repeat')
-- tree.keymap('q', 'quit')
tree.keymap('x', 'execute_system')
tree.keymap('<CR>', 'drop')
tree.keymap('<C-l>', 'redraw')
tree.keymap('<C-g>', 'print')
tree.keymap('>', 'toggle_ignored_files')
tree.keymap('*', 'toggle_select_all')
-- deprecated: multi
-- tree.keymap('s', {'multi', {{'drop', 'split'}, 'quit'}})
tree.keymap('s', {'drop', 'split'}, 'quit')
tree.keymap('s', {'drop', 'split'})
tree.keymap('N', 'new_file')
tree.keymap('cd', {'cd', {'.'}})
tree.keymap('cd', {'cd', '.'})
tree.keymap('~', 'cd')
-- tree action and vim action
tree.keymap('<Tab>', 'toggle_select', 'j')
-- callable need to be evaluated every time
tree.keymap([[\]], {'cd', vim.fn.getcwd})
tree.keymap('cD', {'call', function(context) print(vim.inspect(context)) end})
-- tree.keymap('cD', {'call', vim.fn.CallDemo})
tree.keymap('l', 'open')
tree.keymap('yy', 'yank_path')
tree.keymap('D', 'debug')
tree.keymap('d', 'remove')
tree.keymap('E', {'open', 'vsplit'})
tree.keymap('h', {'cd', '..'})
tree.keymap('gk', {'goto', 'parent'})
