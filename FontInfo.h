/* -*- C++ -*-
 * 
 *  FontInfo.h - Font information storage class of ONScripter
 *
 *  Copyright (c) 2001-2020 Ogapee. All rights reserved.
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __FONT_INFO_H__
#define __FONT_INFO_H__

#include <SDL.h>
#include "BaseReader.h"
#include "Encoding.h"

typedef unsigned char uchar3[3];

class FontInfo{
public:
    enum { YOKO_MODE = 0,
           TATE_MODE = 1
    };
#if defined(INSANI)
    void *ttf_font[8]; // 0...normal rendering, 1...outline rendering, 2...bold rendering, 3...bold outline rendering, 4...italics rendering, 5...italics outline rendering, 6...bold italics rendering, 7...bold italics outline rendering
#else
    void *ttf_font[2]; // 0...normal rendering, 1...outline rendering
#endif
    uchar3 color;
    uchar3 on_color, off_color, nofile_color;
    int font_size_xy[2];
    int top_xy[2]; // Top left origin
    int num_xy[2]; // Row and column of the text windows
    float xy[2]; // Current position
    int old_xy[2];
    int pitch_xy[2]; // Width and height of a character
    int wait_time;
    bool is_bold;
    bool is_shadow;
    bool is_transparent;
    bool is_newline_accepted;
    uchar3  window_color;

#if defined(INSANI)
    bool style_bold;
    bool style_italics;
    bool style_underline;
    bool faux_bold;
    bool faux_italics;
    bool faux_bolditalics;
    int point_size;
#endif

    int line_offset_xy[2]; // ruby offset for each line
    bool rubyon_flag;
    int tateyoko_mode;
    Encoding *enc; // encoding
    bool is_line_space_fixed;

    FontInfo();
    void reset(Encoding *enc);
#if defined(INSANI)
    void *openFont( char *font_file, char* font_bold_file, char* font_italics_file, char* font_bolditalics_file, bool ons_faux_bold, bool ons_faux_italics, bool ons_faux_bolditalics, int ratio1, int ratio2 );
#else
    void *openFont( char *font_file, int ratio1, int ratio2 );
#endif
    void setTateyokoMode( int tateyoko_mode );
    int getTateyokoMode();
    int getRemainingLine();
#if defined(INSANI)
    int getStyle();
    void setStyle(int style, bool bold, bool italics, bool underline);
    inline int getPointSize() { return point_size; }
#endif
    void toggleStyle(int style);
    
    int x(bool use_ruby_offset=true);
    int y(bool use_ruby_offset=true);
    void setXY( int x=-1, int y=-1 );
    void clear();
    void newLine();
    void setLineArea(const char *buf);

    bool isEndOfLine(float margin=0.);
    bool isLineEmpty();
    void advanceCharInHankaku(float offest);
    void addLineOffset(int margin);
    void setRubyOnFlag(bool flag);

    SDL_Rect calcUpdatedArea(int start_xy[2], int ratio1, int ratio2);
    void addShadeArea(SDL_Rect &rect, int dx, int dy, int dw, int dh);
    int initRuby(FontInfo &body_info, int body_count, int ruby_count);
};

#endif // __FONT_INFO_H__
