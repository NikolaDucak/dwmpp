#include "xlib/XFont.h"

namespace xlib {

XCore* XFont::xcore = &XCore::instance();

/* Using the pattern found at font->xfont->pattern does not yield the
 * same substitution results as using the pattern returned by
 * FcNameParse; using the latter results in the desired fallback
 * behaviour whereas the former just results in missing-character
 * rectangles being drawn, at least with some fonts. */
// WARNING: cant use static xcore since it wont initialized it
XFont::XFont(const std::string& fontname) :
    xfont_(XftFontOpenName(XCore::instance().getDpyPtr(),
                           XCore::instance().getScreen(), fontname.c_str())),
    pattern_(FcNameParse(reinterpret_cast<const FcChar8*>(fontname.c_str()))) {
    if (not xfont_) {
        //util::die("XFONT ERR: cant load form name %s", fontname.c_str());
        exit(0);
    }
    if (not pattern_) {
        XftFontClose(xcore->getDpyPtr(), xfont_);
        /*
        util::die("XFONT ERR: cant load name form pattern %s",

                  fontname.c_str());
                  */
        exit(0);
    }

    /* Do not allow using color fonts. This is a workaround for a BadLength
     * error from Xft with color glyphs. Modelled on the Xterm workaround. See
     * https://bugzilla.redhat.com/show_bug.cgi?id=1498269
     * https://lists.suckless.org/dev/1701/30932.html
     * https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=916349
     * and lots more all over the internet.
     */
    FcBool iscol;
    if (FcPatternGetBool(xfont_->pattern, FC_COLOR, 0, &iscol) == FcResultMatch 
        && iscol) {
        XftFontClose(xcore->getDpyPtr(), xfont_);
        //util::die("DIE no gliph");
    }
    this->height_ = xfont_->ascent + xfont_->descent;
}

int XFont::getTextWidthInPixels(const std::string& text) const {
    XGlyphInfo ext;
    XftTextExtentsUtf8(xcore->getDpyPtr(), xfont_, reinterpret_cast<const XftChar8*>(text.c_str()),
                       text.size(), &ext);
    return ext.xOff;
}

} // namespace xlib
