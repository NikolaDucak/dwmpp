#include "wm/layout.h"

namespace wm {

std::ostream& operator << ( std::ostream& os, const util::point& a )
{
    os << "point { " << a.x <<", " << a.y<< " }";
    return os;
}

std::ostream& operator << ( std::ostream& os, const util::area& a )
{
    os << "area { " << a.top_left << ", " << a.width <<", " << a.height<< " }";
    return os;
}

void tiling_layout(util::focus_list<client>& clients, const layout_config& cfg,
                   const util::area& area) {

    if(clients.empty()) return;

    auto number_of_tiled_clients =
        std::count_if(clients.begin(), clients.end(),
                      [](const client& c) { return not c.is_floating(); });

    if (number_of_tiled_clients == 1) {
        clients.front().move_resize({
            area.top_left + cfg.outer_gap,
            area.width - (cfg.outer_gap * 2),
            area.height - (cfg.outer_gap * 2),
        });

    } else if (number_of_tiled_clients > 0) {
        int bw = client::conf.border_width;

        util::point master_current_top_left = area.top_left + util::point {
            (int)cfg.outer_gap - bw,
            (int)cfg.outer_gap - bw,
        };

        util::point master_size {
            (int)(area.width * cfg.master_percentage * 0.01) - (int)(cfg.inner_gap/2) - (int)(2 * cfg.outer_gap),
            (int)((area.height - cfg.outer_gap*2) / cfg.number_of_masters) - (int)(cfg.inner_gap),
        };

        // nuber of slave clients
        number_of_tiled_clients -= cfg.number_of_masters;

        util::point slave_current_top_left = area.top_left + util::point {
            (int)(master_current_top_left.x + cfg.inner_gap + master_size.x) - bw,
            (int)(cfg.outer_gap) - bw,
        };

        util::point slave_size {
            (int)(area.width - master_current_top_left.x - master_size.x) - (int)(cfg.inner_gap) - (int)cfg.outer_gap,
            (int)((area.height - cfg.outer_gap*2) / number_of_tiled_clients) - (int)(cfg.inner_gap),
        };

        uint masters_arranged = 0;
        for (auto& c : clients) {
            c.raise();
            if (c.is_floating()) {
                c.raise();
            } else if (masters_arranged < cfg.number_of_masters) {
                c.move_resize({ master_current_top_left, (uint)master_size.x, (uint)master_size.y });
                master_current_top_left.y += master_size.y + cfg.inner_gap;
                masters_arranged++;
            } else {
                c.move_resize({ slave_current_top_left, (uint)slave_size.x, (uint)slave_size.y });
                slave_current_top_left.y += slave_size.y + cfg.inner_gap;
            }
        }
    }
}

void fullscreen_layout(util::focus_list<client>& clients,
                       const layout_config& cfg, const util::area& area) {

    if(clients.empty()) return;

    for (auto& c : clients) {
        c.move_resize({
            area.top_left + cfg.outer_gap,
            area.width - (cfg.outer_gap * 2),
            area.height - (cfg.outer_gap * 2),
        });
    }

    clients.focused()->raise();
}

}
