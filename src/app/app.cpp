#include "app.h"
#include <cinttypes>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

// Checked
App::App(nvim::Nvim *nvim, int chan_id) : m_nvim(nvim), chan_id(chan_id)
{
    char format[] = "%s: %" PRIu64 "\n";
    printf(format, __FUNCTION__, chan_id);

    // call rpcnotify(g:tree#_channel_id, "_tree_start", "/Users/zgp/")
    auto &a = *m_nvim;
    a.set_var("tree#_channel_id", chan_id);
    Tree::api = m_nvim;

    // init highlight
    char name[40];
    char cmd[80];

    // sprintf(cmd, "silent hi %s guifg=%s", name, cell.color.toStdString().c_str());
    sprintf(name, "tree_%d_0", FILENAME); // file
    sprintf(cmd, "hi %s guifg=%s", name, gui_colors[YELLOW].data());
    a.command(cmd);
    sprintf(name, "tree_%d_1", FILENAME); // dir
    sprintf(cmd, "hi %s guifg=%s", name, gui_colors[BLUE].data());
    a.command(cmd);

    sprintf(name, "tree_%d", SIZE);
    sprintf(cmd, "hi %s guifg=%s", name, gui_colors[GREEN].data());
    a.command(cmd);

    sprintf(name, "tree_%d", TIME);
    sprintf(cmd, "hi %s guifg=%s", name, gui_colors[BLUE].data());
    a.command(cmd);

    for (int i = 0; i < 83; ++i) {
        sprintf(name, "tree_%d_%d", ICON, i);
        sprintf(cmd, "hi %s guifg=%s", name, icons[i][1].data());
        a.command(cmd);
    }
    for (int i = 0; i < 16; ++i) {
        sprintf(name, "tree_%d_%d", MARK, i);
        sprintf(cmd, "hi %s guifg=%s", name, gui_colors[i].data());
        a.command(cmd);
    }
    for (int i = 0; i < 8; ++i) {
        sprintf(name, "tree_%d_%d", GIT, i);
        sprintf(cmd, "hi %s guifg=%s", name, git_indicators[i][1].data());
        a.command(cmd);
    }
}

void App::createTree(string &path)
{
    static int count = 0;
    auto &b = m_nvim;

    int bufnr = b->create_buf(false, true);
    char name[64];
    sprintf(name, "Tree-%d", count);
    string bufname(name);
    b->buf_set_name(bufnr, bufname);
    count++;

    int ns_id = b->create_namespace("tree_icon");
    if (path.back()=='/')  // path("/foo/bar/").parent_path();    // "/foo/bar"
        path.pop_back();
    cout << __FUNCTION__ << " bufnr:" << bufnr << " ns_id:" << ns_id << " path:" << path << endl;

    Tree &tree = *(new Tree(bufnr, ns_id));
    trees.insert({bufnr, &tree});
    treebufs.insert(treebufs.begin(), bufnr);
    tree.cfg.update(m_cfgmap);

    m_ctx.prev_bufnr = bufnr;
    tree.changeRoot(path);

    nvim::Dictionary tree_cfg{
        {"winwidth", tree.cfg.winwidth},
        {"winheight", tree.cfg.winheight},
        {"split", tree.cfg.split.c_str()},
        {"new", tree.cfg.new_},
        {"toggle", tree.cfg.toggle},
        {"direction", tree.cfg.direction.c_str()},
    };
    b->execute_lua("tree.resume(...)", {m_ctx.prev_bufnr, tree_cfg});
}

void App::handleNvimNotification(const string &method, const vector<nvim::Object> &args)
{
    cout << __FUNCTION__ << ":" << method << endl;

    if(method=="_tree_async_action" && args.size() > 0) {
        // _tree_async_action [action: string, args: vector, context: multimap]
        string action = args.at(0).as_string();
        vector<nvim::Object> act_args = args.at(1).as_vector();
        auto context = args.at(2).as_multimap();
        for (auto i : args[2].as_multimap()) {
            cout << i.first.as_string() << ':';
        }
        cout << endl;

        // cout << "\t" << "action:" << action << "args:" << act_args;
        // cout << "context:" << context;
        m_ctx = context;
        // cout << "\t" <<trees<< m_ctx.prev_bufnr;

        auto search = trees.find(m_ctx.prev_bufnr);
        if (search != trees.end()) {
            // if (action == "quit" && args.size() > 0)
            search->second->action(action, act_args, context);
        }
    }
    else if (method=="function") {
        string fn = args.at(0).as_string();
        if (fn == "paste") {
            vector<nvim::Object> fargs = args.at(1).as_vector();
            vector<nvim::Object> pos = fargs[0].as_vector();
            string src = fargs[1].as_string();
            string dest = fargs[2].as_string();
            int buf = pos[0].as_uint64_t();
            int line = pos[1].as_uint64_t();
            // cout<<fargs;
            trees[buf]->paste(line, src, dest);
        }
        else if (fn == "new_file") {
            vector<nvim::Object> fargs = args.at(1).as_vector();
            string input = fargs[0].as_string();
            int bufnr = fargs[1].as_uint64_t();
            trees[bufnr]->handleNewFile(input);
        }
        else if (fn == "rename") {
            vector<nvim::Object> fargs = args.at(1).as_vector();
            string input = fargs[0].as_string();
            int bufnr = fargs[1].as_uint64_t();
            trees[bufnr]->handleRename(input);
        }
        else if (fn == "remove") {
            vector<nvim::Object> fargs = args.at(1).as_vector();
            int buf = fargs[0].as_uint64_t();
            int choice = fargs[1].as_uint64_t();
            // cout << fargs;
            trees[buf]->remove();
        }
        else if (fn == "on_detach") {
            const int buf = args.at(1).as_uint64_t();
            auto got = trees.find(buf);
            if (got != trees.end()) {
                delete trees[buf];
                trees.erase(buf);
                // TODO: 修改tree_buf
                printf("\tAfter remove %d, trees:", buf);
                for (auto i:trees) {
                    cout << i.first << ",";
                }
                cout << endl;
            }
        }
    }
}

void App::handleRequest(nvim::NvimRPC & rpc, uint64_t msgid, const string& method,
        const vector<nvim::Object> &args)
{
    cout << __FUNCTION__ << method << endl;

    auto method_args = args[0].as_vector();
    auto context = args[1].as_multimap();

    if(method=="_tree_start" && args.size() > 0)
    {
        // _tree_start [paths: List, context: Dictionary]
        string path = method_args[0].as_string();
        m_cfgmap = context;
        auto *b = m_nvim;

        auto search = m_cfgmap.find("new");
        if (trees.size()<1 || (search != m_cfgmap.end() && search->second.as_bool())) {
            // TODO: createTree时存在request和此处的response好像产生冲突
            createTree(path);
        }
        else {
            // NOTE: Resume tree buffer by default.
            // TODO: consider to use treebufs[0]
            Tree & tree = *trees[m_ctx.prev_bufnr];

            auto it = std::find(treebufs.begin(), treebufs.end(), m_ctx.prev_bufnr);
            if (it != treebufs.end()) {
                treebufs.erase(it);
                treebufs.insert(treebufs.begin(), m_ctx.prev_bufnr);
            }
            tree.cfg.update(m_cfgmap);

            nvim::Array bufnrs;
            for (const int item : treebufs)
                bufnrs.push_back(item);

            Map tree_cfg = {
                {"winwidth",  tree.cfg.winwidth},
                {"winheight", tree.cfg.winheight},
                {"split", tree.cfg.split.c_str()},
                {"new", tree.cfg.new_},
                {"toggle", tree.cfg.toggle},
                {"direction", tree.cfg.direction.c_str()}
            };

            b->async_execute_lua("tree.resume(...)", {bufnrs, tree_cfg});

            // TODO: columns 状态更新需要清除不需要的columns
        }

        rpc.send_response(msgid, {}, {});
    } else {
        // be sure to return early or this message will be sent
        rpc.send_response(msgid, {"Unknown method"}, {});
    }
}
