#include "xlib/XGraphics.h"
#include <X11/Xlib.h>
#include <iostream>

#define UTF_INVALID 0xFFFD
#define UTF_SIZ 4

#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))

namespace xlib {

// dwms drw.c rip-off
namespace { 

static const unsigned char utfbyte[UTF_SIZ + 1] = { 0x80, 0, 0xC0, 0xE0, 0xF0 };
static const unsigned char utfmask[UTF_SIZ + 1] = { 0xC0, 0x80, 0xE0, 0xF0, 0xF8 };
static const long          utfmin[UTF_SIZ + 1]  = { 0, 0, 0x80, 0x800, 0x10000 };
static const long          utfmax[UTF_SIZ + 1]  = { 0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF };

static long utf8decodebyte(const char c, size_t* i) {
    for (*i = 0; *i < (UTF_SIZ + 1); ++(*i))
        if (((unsigned char)c & utfmask[*i]) == utfbyte[*i])
            return (unsigned char)c & ~utfmask[*i];
    return 0;
}

static size_t utf8validate(long* u, size_t i) {
    if (!BETWEEN(*u, utfmin[i], utfmax[i]) || BETWEEN(*u, 0xD800, 0xDFFF))
        *u = UTF_INVALID;
    for (i = 1; *u > utfmax[i]; ++i);
    return i;
}

static size_t utf8decode(const char* c, long* u, size_t clen) {
    size_t i, j, len, type;
    long   udecoded;

    *u = UTF_INVALID;
    if (!clen) return 0;
    udecoded = utf8decodebyte(c[0], &len);
    if (!BETWEEN(len, 1, UTF_SIZ)) return 1;
    for (i = 1, j = 1; i < clen && j < len; ++i, ++j) {
        udecoded = (udecoded << 6) | utf8decodebyte(c[i], &type);
        if (type) return j;
    }
    if (j < len) return 0;
    *u = udecoded;
    utf8validate(u, len);

    return len;
}

} // anonymous namespace

using util::point;

XCore* XGraphics::xcore = &XCore::instance();

XGraphics::XGraphics() {
    auto dpy_ = xcore->getDpyPtr();
    gc_       = XCreateGC(dpy_, xcore->getRoot(), 0, NULL);
    drawable_ =
        XCreatePixmap(dpy_, xcore->getRoot(),
                      DisplayWidth(xcore->getDpyPtr(), xcore->getScreen()),
                      DisplayHeight(xcore->getDpyPtr(), xcore->getScreen()),
                      DefaultDepth(xcore->getDpyPtr(), xcore->getScreen()));
    XSetLineAttributes(dpy_, gc_, 1, LineSolid, CapButt, JoinMiter);
}

int XGraphics::drawText(point xy, point wh, const std::string& textstr, std::vector<XFont>& fonts, const XColor& clr) {
    auto text = textstr.c_str();
    auto color = clr.get();
    auto dpy = xcore->getDpyPtr();
    auto screen  = DefaultScreen(dpy);
    auto root = xcore->getRoot();
    int  x = xy.x;
    int  y = xy.y;
    int  w = wh.x;
    int  h = wh.y;
    bool charexists = false;
    //auto drawable = XCreatePixmap(dpy, root, 300, 300, DefaultDepth(dpy, screen));
    auto drawable = drawable_;
    auto d = XftDrawCreate(dpy, drawable, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen));
    bool render = w || h;
    XFont* usedfont, *curfont, *nextfont;

    usedfont = fonts.data();
    if(!render)
        w = ~w;

    while (1) {
        char     buf[1024];
        long     utf8codepoint = 0;
        int      utf8charlen, utf8strlen = 0;
        auto     utf8str    = text;
        nextfont = NULL;

        // for each char
        while (*text) {
            utf8charlen = utf8decode(text, &utf8codepoint, UTF_SIZ);
            // for each font
            for(auto& cfont : fonts) {
                curfont = &cfont;
                charexists = charexists || XftCharExists(dpy, curfont->get(), utf8codepoint);
                if (charexists) {
                    if (curfont == usedfont) {
                        utf8strlen += utf8charlen;
                        text += utf8charlen;
                    } else {
                        nextfont = curfont;
                    }
                    break;
                }
            }

            if (!charexists || nextfont)
                break;
            else
                charexists = false;
            
        }

        if (utf8strlen) {
            auto ew = usedfont->getTextExtents(utf8str,utf8strlen);
            int len;
            /* shorten text if necessary */
            for (len = MIN(utf8strlen, sizeof(buf) - 1); len && ew > w; len--)
                ew = usedfont->getTextExtents(utf8str,utf8strlen);

            if (len) {
                memcpy(buf, utf8str, len);
                buf[len] = '\0';
                if (len < utf8strlen)
                    for (auto i = len; i && i > len - 3; buf[--i] = '.') ; /* NOP */

                auto ty = y + (h - usedfont->getHeight()) / 2 + usedfont->get()->ascent;
                if(render)
                    XftDrawStringUtf8(d, &color, usedfont->get(), x, ty, (XftChar8*)buf, len);
                x += ew;
                w -= ew;
            }
        }

        if (!*text) {
            break;
        } else if (nextfont) {
            charexists = 0;
            usedfont = nextfont;
        } else {
            /* Regardless of whether or not a fallback font is found, the
             * character must be drawn. */
            charexists = 1;

            FcCharSet*   fccharset = FcCharSetCreate();
            FcCharSetAddChar(fccharset, utf8codepoint);

            if (!fonts.front().getPattern()) {
                /* Refer to the comment in xfont_create for more information. */
                //util::die("the first font in the cache must be loaded from a font string.");
            }

            FcPattern* fcpattern= FcPatternDuplicate(fonts.front().getPattern());
            FcPatternAddCharSet(fcpattern, FC_CHARSET, fccharset);
            FcPatternAddBool(fcpattern, FC_SCALABLE, FcTrue);
            FcPatternAddBool(fcpattern, FC_COLOR, FcFalse);

            FcConfigSubstitute(NULL, fcpattern, FcMatchPattern);
            FcDefaultSubstitute(fcpattern);

            XftResult result;
            auto match = XftFontMatch(dpy, screen, fcpattern, &result);

            FcCharSetDestroy(fccharset);
            FcPatternDestroy(fcpattern);

            if (match) {
                usedfont = new XFont(match);
                if (usedfont && XftCharExists(dpy, usedfont->get(), utf8codepoint)) {
                    fonts.push_back(*usedfont);
                } else {
                    delete usedfont;
                    usedfont = &fonts[0];
                }
            }
        }
    }
    if (d) {
        XftDrawDestroy(d);
    }
    
    return x + (render ? w : 0);
}

int XGraphics::getTextWidth(const std::string& text, std::vector<XFont>& fonts) {
    static XColor dummyColor {"#000000"};
    return drawText(point{0,0},point{0,0}, text, fonts, dummyColor);

};


void XGraphics::fillRectangle(const xlib::XColor& c, point xy, point wh) {
    auto dpy_ = xcore->getDpyPtr();
    XSetForeground(dpy_, gc_, c.get().pixel);
    XFillRectangle(dpy_, drawable_, gc_, xy.x, xy.y, wh.x, wh.y);
}

void XGraphics::copyArea(Window w, point xy, point wh) {
    Display* dpy_ = xcore->getDpyPtr();
    XCopyArea(dpy_, drawable_, w, gc_, xy.x, xy.y, wh.x, wh.y, xy.x, xy.y);
    XSync(dpy_, False);
}

}  // namespace xlib
