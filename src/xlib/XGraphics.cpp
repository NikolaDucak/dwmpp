#include "xlib/XGraphics.h"
#include <X11/Xlib.h>

namespace xlib {


#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))
#define UTF_INVALID 0xFFFD
#define UTF_SIZ 4

static const unsigned char utfbyte[UTF_SIZ + 1] = { 0x80, 0, 0xC0, 0xE0, 0xF0 };
static const unsigned char utfmask[UTF_SIZ + 1] = { 0xC0, 0x80, 0xE0, 0xF0,
                                                    0xF8 };
static const long          utfmin[UTF_SIZ + 1] = { 0, 0, 0x80, 0x800, 0x10000 };
static const long utfmax[UTF_SIZ + 1] = { 0x10FFFF, 0x7F, 0x7FF, 0xFFFF,
                                          0x10FFFF };

static long utf8decodebyte(const char c, size_t* i) {
    for (*i = 0; *i < (UTF_SIZ + 1); ++(*i))
        if (((unsigned char)c & utfmask[*i]) == utfbyte[*i])
            return (unsigned char)c & ~utfmask[*i];
    return 0;
}

static size_t utf8validate(long* u, size_t i) {
    if (!BETWEEN(*u, utfmin[i], utfmax[i]) || BETWEEN(*u, 0xD800, 0xDFFF))
        *u = UTF_INVALID;
    for (i = 1; *u > utfmax[i]; ++i)
        ;
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

void XGraphics::drawText(const XFont& fnt, const XColor& clr, point xy,
                         const std::string& text) {
    Display* dpy_ = xcore->getDpyPtr();
    int      scr  = DefaultScreen(dpy_);
    int      ty   = xy.y + fnt.get()->ascent;  // correct text position
    XftDraw* draw = XftDrawCreate(dpy_, drawable_, DefaultVisual(dpy_, scr),
                                  DefaultColormap(dpy_, scr));
    XftDrawStringUtf8(draw, &clr.get(), const_cast<XftFont*>(fnt.get()), xy.x, ty, (const FcChar8*)text.c_str(), text.size());

    XftDrawDestroy(draw);
}

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

void XGraphics::dwmDrawtext(int x, int y, unsigned int w, unsigned int h,
             unsigned int lpad, const char* text, int invert) {
    auto* dpy_ = xcore->getDpyPtr();
    char         buf[1024];
    int          ty;
    unsigned int ew;
    XftDraw*     d = NULL;
    //Fnt *        usedfont, *curfont, *nextfont;
    size_t       i, len;
    int          utf8strlen, utf8charlen, render = x || y || w || h;
    long         utf8codepoint = 0;
    const char*  utf8str;
    FcCharSet*   fccharset;
    FcPattern*   fcpattern;
    FcPattern*   match;
    XftResult    result;
    int          charexists = 0;

    if (!render) {
        w = ~w;
    } else {
        d = XftDrawCreate(dpy_, drawable_,
                          DefaultVisual(dpy_, screen_),
                          DefaultColormap(dpy_, screen_));
        x += lpad;
        w -= lpad;
    }

    usedfont = drw->fonts;
    while (1) {
        utf8strlen = 0;
        utf8str    = text;
        nextfont   = NULL;
        while (*text) {
            utf8charlen = utf8decode(text, &utf8codepoint, UTF_SIZ);
            for (curfont = drw->fonts; curfont; curfont = curfont->next) {
                charexists =
                    charexists ||
                    XftCharExists(drw->dpy, curfont->xfont, utf8codepoint);
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
                charexists = 0;
        }

        if (utf8strlen) {
            drw_font_getexts(usedfont, utf8str, utf8strlen, &ew, NULL);
            /* shorten text if necessary */
            for (len = MIN(utf8strlen, sizeof(buf) - 1); len && ew > w; len--)
                drw_font_getexts(usedfont, utf8str, len, &ew, NULL);

            if (len) {
                memcpy(buf, utf8str, len);
                buf[len] = '\0';
                if (len < utf8strlen)
                    for (i = len; i && i > len - 3; buf[--i] = '.')
                        ; /* NOP */

                if (render) {
                    ty = y + (h - usedfont->h) / 2 + usedfont->xfont->ascent;
                    XftDrawStringUtf8(d, &drw->scheme[invert ? ColBg : ColFg],
                                      usedfont->xfont, x, ty, (XftChar8*)buf,
                                      len);
                }
                x += ew;
                w -= ew;
            }
        }

        if (!*text) {
            break;
        } else if (nextfont) {
            charexists = 0;
            usedfont   = nextfont;
        } else {
            /* Regardless of whether or not a fallback font is found, the
             * character must be drawn. */
            charexists = 1;

            fccharset = FcCharSetCreate();
            FcCharSetAddChar(fccharset, utf8codepoint);

            if (!drw->fonts->pattern) {
                /* Refer to the comment in xfont_create for more information. */
                die("the first font in the cache must be loaded from a font "
                    "string.");
            }

            fcpattern = FcPatternDuplicate(drw->fonts->pattern);
            FcPatternAddCharSet(fcpattern, FC_CHARSET, fccharset);
            FcPatternAddBool(fcpattern, FC_SCALABLE, FcTrue);
            FcPatternAddBool(fcpattern, FC_COLOR, FcFalse);

            FcConfigSubstitute(NULL, fcpattern, FcMatchPattern);
            FcDefaultSubstitute(fcpattern);
            match = XftFontMatch(drw->dpy, drw->screen, fcpattern, &result);

            FcCharSetDestroy(fccharset);
            FcPatternDestroy(fcpattern);

            if (match) {
                usedfont = xfont_create(drw, NULL, match);
                if (usedfont &&
                    XftCharExists(drw->dpy, usedfont->xfont, utf8codepoint)) {
                    for (curfont = drw->fonts; curfont->next;
                         curfont = curfont->next)
                        ; /* NOP */
                    curfont->next = usedfont;
                } else {
                    xfont_free(usedfont);
                    usedfont = drw->fonts;
                }
            }
        }
    }
    if (d) XftDrawDestroy(d);

    return x + (render ? w : 0);
}

}  // namespace xlib
