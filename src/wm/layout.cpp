#include "wm/layout.h"

namespace wm {

void tiling_layout(util::focus_list<client>& clients, const layout_config& cfg,
                   const util::rect& area) {
    int outer_gaps_enable = 1, inner_gaps_enable = 1;  // TODO: get from config
    int gappiv = cfg.inner_gap * inner_gaps_enable;
    int gappih = cfg.inner_gap * inner_gaps_enable;
    int gappoh = cfg.outer_gap * outer_gaps_enable;
    int gappov = cfg.outer_gap * outer_gaps_enable;
    int master_w, master_y = gappoh;
    int slave_y = gappoh;

    float mfact = cfg.master_percentage / 100.f;

    uint n = std::count_if(clients.begin(), clients.end(),
                           [](const client& c) { return not c.is_floating(); });

    // if there are no tiled clients, then there is nothing to do
    if (!n) return;

    if (n > cfg.number_of_masters)
        master_w = cfg.number_of_masters ? (area.width + gappiv) * mfact : 0;
    else
        master_w = area.width - 2 * gappov + gappiv;

    util::rect master_area { { area.top_left.x + gappov, /* placeholder */ 0 },
                             master_w - (2 * client::conf.border_width) -
                                 gappiv,
                             /* placeholder */ 0 };

    util::rect slave_area { { area.top_left.x + master_w + gappov,
                              /* placehloder */ area.top_left.y + gappoh },
                            area.width - master_w -
                                (2 * cfg.number_of_masters) - 2 * gappov,
                            /* placehloder */ 0 };

    // counter for counting how many clients are arranged
    // used to check if appropriate number of clients
    // is arranged in the master area
    size_t i = 0;
    for (auto& c : clients) {
        int r, h;
        // floating clients are just brougth to top
        // TODO: maybe traverse clients backwards so
        // floating windows closer to client stack top are
        // raised above the others that are lower
        if (c.is_floating()) {
            c.raise();
            continue;  // skip `i++` line at the bottom of the loop
        } else if (i < cfg.number_of_masters) { 
            r = std::min(n, cfg.number_of_masters) - i;
            h = (area.height - master_y - gappoh - gappih * (r - 1)) / r;
            master_area.top_left.y = area.top_left.y + master_y;
            master_area.height     = h - (2 * client::conf.border_width);
            c.move_resize(master_area);
            master_y += c.get_size().y + 2 * client::conf.border_width + gappih;
        // if we've arrange enough master then 
        } else {
            r = n - i;
            h = (area.height - client::conf.border_width - slave_y - gappoh -
                 gappih * (r - 1)) / r;
            slave_area.top_left.y = area.top_left.y + slave_y;
            slave_area.height     = h - (2 * cfg.number_of_masters);
            c.move_resize(slave_area);
            slave_y += c.get_size().y + 2 * client::conf.border_width + gappih;
        }
        i++;
    }
}

void fullscreen_layout(util::focus_list<client>& clients,
                       const layout_config& cfg, const util::rect& area) {
    if (clients.empty()) return;

    for (auto& c : clients) {
        if (c.is_floating()) continue;
        //TODO: (int) cast removes -Wnarrowing warining,
        // find a better way
        c.move_resize({
            area.top_left + (int)cfg.outer_gap, 
            area.width - (cfg.outer_gap * 2),
            area.height - (cfg.outer_gap * 2),
        });
    }

    clients.focused()->raise();
}

}  // namespace wm
