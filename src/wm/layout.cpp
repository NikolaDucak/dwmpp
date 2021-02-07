#include "wm/layout.h"

namespace wm {
//TODO: remove c-style casting
void tiling_layout(util::focus_list<client>& clients, const layout_config& cfg,
                   const util::rect& area) {
    int oe = 1, ie = 1;
    int ww = area.width;
    int workspace_h = area.height;
    int wx = area.top_left.x;
    int wy = area.top_left.y;
    int gappiv=cfg.inner_gap*ie;
    int gappih=cfg.inner_gap*ie;
    int gappoh=cfg.outer_gap*oe;
    int gappov=cfg.outer_gap*oe;

    float mfact = cfg.master_percentage/100.f;

    unsigned n = std::count_if(clients.begin(), clients.end(),
                      [](const client& c) { return not c.is_floating(); });

    if (!n) return;

    int master_w, master_y = gappoh;
    int slave_y = gappoh;

    if (n > cfg.number_of_masters)
        master_w = cfg.number_of_masters ? (ww + gappiv) * mfact : 0;
    else
        master_w = ww - 2 * gappov + gappiv;

    size_t i = 0;
    for (auto& c : clients) {
        int r,h;
        if (c.is_floating()) {
            c.raise();
            continue; // skip `i++` line at the bottom of the loop
        } else if (i < cfg.number_of_masters) {
            r = std::min(n, cfg.number_of_masters) - i;
            h = (workspace_h - master_y - gappoh - gappih * (r - 1)) / r;
            util::rect s { { wx + gappov, wy + master_y },
                           (unsigned)master_w - (2 * (int)client::conf.border_width) - gappiv,
                           (unsigned)h - (2 * (int)client::conf.border_width) };
            c.move_resize(s);
            master_y += c.get_size().y + 2 * client::conf.border_width + gappih;
        } else {
            r = n - i;
            h = (workspace_h - client::conf.border_width - slave_y - gappoh - gappih * (r - 1)) / r;
            util::rect s { { wx + master_w + gappov, wy + slave_y },
                           (unsigned)ww - master_w - (2 * (int)cfg.number_of_masters) - 2 * gappov,
                           (unsigned)h - (2 * (int)cfg.number_of_masters) };
            c.move_resize(s);
            slave_y += c.get_size().y + 2 * client::conf.border_width + gappih;
        }
        i++;
    }
    std::cout << "HERE"<< std::endl;
}
void fullscreen_layout(util::focus_list<client>& clients,
                       const layout_config& cfg, const util::rect& area) {
    if (clients.empty()) return;

    for (auto& c : clients) {
        if (c.is_floating()) continue;
        c.move_resize({
            area.top_left + cfg.outer_gap,
            area.width - (cfg.outer_gap * 2),
            area.height - (cfg.outer_gap * 2),
        });
    }

    clients.focused()->raise();
}

}  // namespace wm
