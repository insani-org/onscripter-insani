/* -*- C++ -*-
 * 
 *  ONScripter_text.cpp - Text parser of ONScripter
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

#include "ONScripter.h"

extern unsigned short convSJIS2UTF16( unsigned short in );

#define IS_ROTATION_REQUIRED(x)	\
    (!IS_TWO_BYTE(*(x)) ||                                              \
     (*(x) == (char)0x81 && *((x)+1) == (char)0x50) ||                  \
     (*(x) == (char)0x81 && *((x)+1) == (char)0x51) ||                  \
     (*(x) == (char)0x81 && *((x)+1) >= 0x5b && *((x)+1) <= 0x5d) ||    \
     (*(x) == (char)0x81 && *((x)+1) >= 0x60 && *((x)+1) <= 0x64) ||    \
     (*(x) == (char)0x81 && *((x)+1) >= 0x69 && *((x)+1) <= 0x7a) ||    \
     (*(x) == (char)0x81 && *((x)+1) == (char)0x80) )

#define IS_TRANSLATION_REQUIRED(x)	\
        ( *(x) == (char)0x81 && *((x)+1) >= 0x41 && *((x)+1) <= 0x44 )

void ONScripter::shiftHalfPixelX(SDL_Surface *surface)
{
    SDL_LockSurface( surface );
    unsigned char *buf = (unsigned char*)surface->pixels;
    for (int i=surface->h ; i!=0 ; i--){
        unsigned char c = buf[0];
        for (int j=1 ; j<surface->w ; j++){
            buf[j-1] = (buf[j]+c)>>1;
            c = buf[j];
        }
        buf[surface->w-1] = c>>1;
        buf += surface->pitch;
    }
    SDL_UnlockSurface( surface );
}

void ONScripter::shiftHalfPixelY(SDL_Surface *surface)
{
    SDL_LockSurface( surface );
    for (int j=surface->w-1 ; j>=0 ; j--){
        unsigned char *buf = (unsigned char*)surface->pixels + j;
        unsigned char c = buf[0];
        for (int i=1 ; i<surface->h ; i++){
            buf += surface->pitch;
            *(buf-surface->pitch) = (*buf+c)>>1;
            c = *buf;
        }
        *buf = c>>1;
    }
    SDL_UnlockSurface( surface );
}

int ONScripter::drawGlyph( SDL_Surface *dst_surface, FontInfo *info, SDL_Color &color, char* text, int xy[2], AnimationInfo *cache_info, SDL_Rect *clip, SDL_Rect &dst_rect )
{
    unsigned short unicode = script_h.enc.getUTF16(text);

    int minx, maxx, miny, maxy, advanced;
#if 0
    if (TTF_GetFontStyle( (TTF_Font*)info->ttf_font[0] ) !=
        (info->is_bold?TTF_STYLE_BOLD:TTF_STYLE_NORMAL) )
        TTF_SetFontStyle( (TTF_Font*)info->ttf_font[0], (info->is_bold?TTF_STYLE_BOLD:TTF_STYLE_NORMAL));
#endif    
    TTF_GlyphMetrics( (TTF_Font*)info->ttf_font[0], unicode,
                      &minx, &maxx, &miny, &maxy, &advanced );
    //printf("min %d %d %d %d %d %d\n", minx, maxx, miny, maxy, advanced,TTF_FontAscent((TTF_Font*)info->ttf_font[0])  );

    static SDL_Color fcol={0xff, 0xff, 0xff}, bcol={0, 0, 0};
    SDL_Surface *tmp_surface = TTF_RenderGlyph_Shaded((TTF_Font*)info->ttf_font[0], unicode, fcol, bcol);
    
    SDL_Color scolor = {0, 0, 0};
    SDL_Surface *tmp_surface_s = tmp_surface;
    if (info->is_shadow && render_font_outline){
        unsigned char max_color = color.r;
        if (max_color < color.g) max_color = color.g;
        if (max_color < color.b) max_color = color.b;
        if (max_color < 0x80) scolor.r = 0xff;
        else                  scolor.r = 0;
        scolor.g = scolor.b = scolor.r;

        tmp_surface_s = TTF_RenderGlyph_Shaded((TTF_Font*)info->ttf_font[1], unicode, fcol, bcol);
        if (tmp_surface && tmp_surface_s){
            if ((tmp_surface_s->w-tmp_surface->w) & 1) shiftHalfPixelX(tmp_surface_s);
            if ((tmp_surface_s->h-tmp_surface->h) & 1) shiftHalfPixelY(tmp_surface_s);
        }
    }

    bool rotate_flag = false;
    if ( info->getTateyokoMode() == FontInfo::TATE_MODE && IS_ROTATION_REQUIRED(text) ) rotate_flag = true;
    
    dst_rect.x = xy[0] + minx;
    dst_rect.y = xy[1] + TTF_FontAscent((TTF_Font*)info->ttf_font[0]) - maxy;
    if (script_h.enc.getEncoding() == Encoding::CODE_CP932)
        dst_rect.y -= (TTF_FontHeight((TTF_Font*)info->ttf_font[0]) - info->font_size_xy[1]*screen_ratio1/screen_ratio2)/2;

    if ( rotate_flag ) dst_rect.x += miny - minx;
        
    if ( info->getTateyokoMode() == FontInfo::TATE_MODE && IS_TRANSLATION_REQUIRED(text) ){
        dst_rect.x += info->font_size_xy[0]/2;
        dst_rect.y -= info->font_size_xy[0]/2;
    }

    if (info->is_shadow && tmp_surface_s){
        SDL_Rect dst_rect_s = dst_rect;
        if (render_font_outline){
            dst_rect_s.x -= (tmp_surface_s->w - tmp_surface->w)/2;
            dst_rect_s.y -= (tmp_surface_s->h - tmp_surface->h)/2;
        }
        else{
            dst_rect_s.x += shade_distance[0];
            dst_rect_s.y += shade_distance[1];
        }

        if (rotate_flag){
            dst_rect_s.w = tmp_surface_s->h;
            dst_rect_s.h = tmp_surface_s->w;
        }
        else{
            dst_rect_s.w = tmp_surface_s->w;
            dst_rect_s.h = tmp_surface_s->h;
        }

        if (cache_info)
            cache_info->blendText( tmp_surface_s, dst_rect_s.x, dst_rect_s.y, scolor, clip, rotate_flag );
        
        if (dst_surface)
            alphaBlendText( dst_surface, dst_rect_s, tmp_surface_s, scolor, clip, rotate_flag );
    }

    if ( tmp_surface ){
        if (rotate_flag){
            dst_rect.w = tmp_surface->h;
            dst_rect.h = tmp_surface->w;
        }
        else{
            dst_rect.w = tmp_surface->w;
            dst_rect.h = tmp_surface->h;
        }

        if (cache_info)
            cache_info->blendText( tmp_surface, dst_rect.x, dst_rect.y, color, clip, rotate_flag );
        
        if (dst_surface)
            alphaBlendText( dst_surface, dst_rect, tmp_surface, color, clip, rotate_flag );
    }

    if (tmp_surface_s && tmp_surface_s != tmp_surface)
        SDL_FreeSurface(tmp_surface_s);
    if (tmp_surface)
        SDL_FreeSurface(tmp_surface);
    
    return advanced;
}

void ONScripter::openFont(FontInfo *fi)
{
    if (fi->ttf_font[0] == NULL){
        if (fi->openFont(font_file, screen_ratio1, screen_ratio2) == NULL){
            fprintf( stderr, "can't open font file: %s\n", font_file );
            quit();
            exit(-1);
        }
    }
#if defined(PSP)
    else
        fi->openFont(font_file, screen_ratio1, screen_ratio2);
#endif
}

void ONScripter::drawChar( char* text, FontInfo *info, bool flush_flag, bool lookback_flag, SDL_Surface *surface, AnimationInfo *cache_info, SDL_Rect *clip )
{
    //printf("draw %x-%x[%s] %d, %d\n", text[0], text[1], text, info->xy[0], info->xy[1] );
    openFont(info);

    if (info->isEndOfLine()){
        info->newLine();
        for (int i=0 ; i<indent_offset ; i++){
            if (lookback_flag){
                if (script_h.enc.getEncoding() == Encoding::CODE_CP932){
                    current_page->add(0x81);
                    current_page->add(0x40);
                }
                else{
                    current_page->add(0xe3);
                    current_page->add(0x80);
                    current_page->add(0x80);
                }
            }
            info->advanceCharInHankaku(2);
        }
    }

    info->old_xy[0] = info->x(false);
    info->old_xy[1] = info->y(false);

    char text2[4] = {};
    int n = script_h.enc.getBytes(text[0]);
    for (int i=0; i<n; i++)
        text2[i] = text[i];

    for (int i=0 ; i<2 ; i++){
        int xy[2];
        xy[0] = info->x() * screen_ratio1 / screen_ratio2;
        xy[1] = info->y() * screen_ratio1 / screen_ratio2;
    
        SDL_Color color = {info->color[0], info->color[1], info->color[2]};
        SDL_Rect dst_rect;
        float adv = drawGlyph(surface, info, color, text2, xy, cache_info, clip, dst_rect); 
        if (n == 1) adv -= 0.5; // 0.5 is for adjusting the increse by FT_CEIL

        if ( surface == accumulation_surface &&
             !flush_flag &&
             (!clip || AnimationInfo::doClipping( &dst_rect, clip ) == 0) ){
            dirty_rect.add( dst_rect );
        }
        else if ( flush_flag ){
            if (info->is_shadow){
                if (render_font_outline)
                    info->addShadeArea(dst_rect, -1, -1, 2, 2);
                else
                    info->addShadeArea(dst_rect, 0, 0, shade_distance[0], shade_distance[1]);
            }
            flushDirect( dst_rect, REFRESH_NONE_MODE );
        }

        if (n >= 2){
            if (script_h.enc.getEncoding() == Encoding::CODE_UTF8){
                // handle a proportional font
                adv += info->pitch_xy[0] - info->font_size_xy[0];
                info->advanceCharInHankaku(adv*2.0/info->pitch_xy[0]);
            }
            else
                info->advanceCharInHankaku(2);
            break;
        }
        if (script_h.enc.getEncoding() == Encoding::CODE_UTF8){
            // handle a proportional font
            adv += info->pitch_xy[0] - info->font_size_xy[0];
            info->advanceCharInHankaku(adv*2.0/info->pitch_xy[0]);
        }
        else
            info->advanceCharInHankaku(1);
        text2[0] = text[1];
        if (text2[0] == 0) break;
    }

    if (lookback_flag){
        for (int i=0; i<n; i++)
            current_page->add(text[i]);
    }
}

void ONScripter::drawString(const char *str, uchar3 color, FontInfo *info, bool flush_flag, SDL_Surface *surface, SDL_Rect *rect, AnimationInfo *cache_info, bool pack_hankaku)
{
    int i;

    int start_xy[2];
    start_xy[0] = info->xy[0];
    start_xy[1] = info->xy[1];

    /* ---------------------------------------- */
    /* Draw selected characters */
    uchar3 org_color;
    for ( i=0 ; i<3 ; i++ ) org_color[i] = info->color[i];
    for ( i=0 ; i<3 ; i++ ) info->color[i] = color[i];

    bool skip_whitespace_flag = true;
    if (script_h.enc.getEncoding() == Encoding::CODE_UTF8)
        skip_whitespace_flag = false;
    char text[4] = {};
    while(*str){
        while (*str == ' ' && skip_whitespace_flag) str++;

#ifdef ENABLE_1BYTE_CHAR
        if ( *str == '`' ){
            str++;
            skip_whitespace_flag = false;
            continue;
        }
#endif

        if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
            *str == script_h.enc.getTextMarker()){
            str++;
            continue;
        }

#ifndef FORCE_1BYTE_CHAR            
        if (cache_info && !cache_info->is_tight_region){
            if (*str == '('){
                startRuby(str+1, *info);
                str++;
                continue;
            }
            else if (*str == '/' && ruby_struct.stage == RubyStruct::BODY ){
                info->addLineOffset(ruby_struct.margin);
                str = ruby_struct.ruby_end;
                if (*ruby_struct.ruby_end == ')'){
                    endRuby(false, false, NULL, cache_info);
                    str++;
                }
                continue;
            }
            else if (*str == ')' && ruby_struct.stage == RubyStruct::BODY ){
                ruby_struct.stage = RubyStruct::NONE;
                str++;
                continue;
            }
            else if (*str == '<'){
                str++;
                int no = 0;
                while(*str>='0' && *str<='9')
                    no=no*10+(*str++)-'0';
                in_textbtn_flag = true;
                continue;
            }
            else if (*str == '>' && in_textbtn_flag){
                str++;
                in_textbtn_flag = false;
                continue;
            }
        }
#endif
        int n = script_h.enc.getBytes(*str);
        if (n >= 2){
            if (checkLineBreak(str, info)){
                info->newLine();
                for (int i=0 ; i<indent_offset ; i++)
                    info->advanceCharInHankaku(2);
            }

            for (int i=0; i<n; i++)
                text[i] = *str++;
            drawChar(text, info, false, false, surface, cache_info);
        }
        else if (*str == 0x0a || (*str == '\\' && info->is_newline_accepted)){
            info->newLine();
            str++;
        }
        else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
                 *str == '~'){
            while(1){
                str++;
                if (*str == 0x0a || *str == 0) break;
                if (*str == '~'){
                    str++;
                    break;
                }
                if (*str == 'i'){
                    openFont(info);
                    info->toggleStyle(TTF_STYLE_ITALIC);
                }
                else if (*str == 'b'){
                    openFont(info);
                    info->toggleStyle(TTF_STYLE_BOLD);
                }
            }
            continue;
        }
        else if (*str){
            if (checkLigatureLineBreak(str, info))
                info->newLine();
            text[0] = *str++;
            if (*str && *str != 0x0a && pack_hankaku &&
                script_h.enc.getEncoding() == Encoding::CODE_CP932)
                text[1] = *str++;
            else
                text[1] = 0;
            drawChar( text, info, false, false, surface, cache_info );
        }
    }
    for ( i=0 ; i<3 ; i++ ) info->color[i] = org_color[i];

    /* ---------------------------------------- */
    /* Calculate the area of selection */
    SDL_Rect clipped_rect = info->calcUpdatedArea(start_xy, screen_ratio1, screen_ratio2);

    SDL_Rect scaled_clipped_rect;
    scaled_clipped_rect.x = clipped_rect.x * screen_ratio1 / screen_ratio2;
    scaled_clipped_rect.y = clipped_rect.y * screen_ratio1 / screen_ratio2;
    scaled_clipped_rect.w = clipped_rect.w * screen_ratio1 / screen_ratio2;
    scaled_clipped_rect.h = clipped_rect.h * screen_ratio1 / screen_ratio2;

    if (info->is_shadow){
        if (render_font_outline)
            info->addShadeArea(scaled_clipped_rect, -1, -1, 2, 2);
        else
            info->addShadeArea(scaled_clipped_rect, 0, 0, shade_distance[0], shade_distance[1]);
    }
    
    if (flush_flag)
        flush(refresh_shadow_text_mode, &scaled_clipped_rect);
    
    if (rect) *rect = clipped_rect;
}

void ONScripter::restoreTextBuffer(SDL_Surface *surface)
{
    text_info.fill( 0, 0, 0, 0 );

    char out_text[4] = {};
    FontInfo f_info = sentence_font;
    f_info.clear();
    for (int i=0 ; i<current_page->text_count ; i++){
        if (current_page->text[i] == 0x0a){
            f_info.newLine();
        }
        else{
            out_text[0] = current_page->text[i];
#if defined(INSANI)
            if ( out_text[0] == '\n' ){
                if(f_info.is_newline_accepted == false) f_info.is_newline_accepted = true;
                f_info.newLine();
                continue;
            }
#endif
#ifndef FORCE_1BYTE_CHAR            
            if (out_text[0] == '('){
                startRuby(current_page->text + i + 1, f_info);
                continue;
            }
            else if (out_text[0] == '/' && ruby_struct.stage == RubyStruct::BODY ){
                f_info.addLineOffset(ruby_struct.margin);
                i = ruby_struct.ruby_end - current_page->text - 1;
                if (*ruby_struct.ruby_end == ')'){
                    endRuby(false, false, surface, &text_info);
                    i++;
                }
                continue;
            }
            else if (out_text[0] == ')' && ruby_struct.stage == RubyStruct::BODY ){
                ruby_struct.stage = RubyStruct::NONE;
                continue;
            }
            else if (out_text[0] == '<'){
                int no = 0;
                while(current_page->text[i+1]>='0' && current_page->text[i+1]<='9')
                    no=no*10+current_page->text[(i++)+1]-'0';
                in_textbtn_flag = true;
                continue;
            }
            else if (out_text[0] == '>' && in_textbtn_flag){
                in_textbtn_flag = false;
                continue;
            }
#endif

            int n = script_h.enc.getBytes(out_text[0]);
            if (n >= 2){
                for (int j=1 ; j<n; j++)
                    out_text[j] = current_page->text[i+j];
                
                if (checkLineBreak(current_page->text+i, &f_info))
                    f_info.newLine();
                i += n-1;
            }
            else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
                     out_text[0] == '~'){
                while(1){
                    char ch = current_page->text[++i];
                    if (ch == 0x0a || ch == 0) break;
                    if (ch == '~'){
                        i++;
                        break;
                    }
                    if (ch == 'i'){
                        openFont(&f_info);
                        f_info.toggleStyle(TTF_STYLE_ITALIC);
                    }
                    else if (ch == 'b'){
                        openFont(&f_info);
                        f_info.toggleStyle(TTF_STYLE_BOLD);
                    }
                }
                continue;
            }
            else{
                if (checkLigatureLineBreak(current_page->text+i, &f_info))
                    f_info.newLine();
                
                out_text[1] = 0;
                
                if (i+1 != current_page->text_count &&
                    current_page->text[i+1] != 0x0a &&
                    script_h.enc.getEncoding() == Encoding::CODE_CP932){
                    out_text[1] = current_page->text[i+1];
                    i++;
                }
            }
            drawChar(out_text, &f_info, false, false, surface, &text_info);
        }
    }
}

void ONScripter::enterTextDisplayMode(bool text_flag)
{
    if (line_enter_status <= 1 && (!pretextgosub_label || saveon_flag) && internal_saveon_flag && text_flag){
        storeSaveFile();
        internal_saveon_flag = false;
    }
    
    if (!(display_mode & DISPLAY_MODE_TEXT)){
        dirty_rect.clear();
        dirty_rect.add( sentence_font_info.pos );
        display_mode = DISPLAY_MODE_TEXT;

        if (setEffect(&window_effect)) return;
        while(doEffect(&window_effect, false));

        text_on_flag = true;
    }
}

void ONScripter::leaveTextDisplayMode(bool force_leave_flag)
{
    if (display_mode & DISPLAY_MODE_TEXT &&
        (force_leave_flag || erase_text_window_mode != 0)){

        dirty_rect.add(sentence_font_info.pos);
        display_mode = DISPLAY_MODE_NORMAL;
            
        if (setEffect(&window_effect)) return;
        while(doEffect(&window_effect, false));
    }
}

bool ONScripter::doClickEnd()
{
    bool ret = false;

    refresh_shadow_text_mode |= REFRESH_CURSOR_MODE;

    if ( automode_flag ){
        event_mode =  WAIT_TEXT_MODE | WAIT_INPUT_MODE | WAIT_VOICE_MODE | WAIT_TIMER_MODE;
        if ( automode_time < 0 )
            ret = waitEvent( -automode_time * num_chars_in_sentence );
        else
            ret = waitEvent( automode_time );
    }
    else if ( autoclick_time > 0 ){
        event_mode = WAIT_TIMER_MODE;
        ret = waitEvent( autoclick_time );
    }
    else{
        event_mode = WAIT_TEXT_MODE | WAIT_INPUT_MODE | WAIT_TIMER_MODE;
        ret = waitEvent(-1);
    }

    num_chars_in_sentence = 0;
    
    refresh_shadow_text_mode &= ~REFRESH_CURSOR_MODE;
    stopAnimation( clickstr_state );
    
    return ret;
}

bool ONScripter::clickWait( char *out_text )
{
    flush( REFRESH_NONE_MODE );
    skip_mode &= ~SKIP_TO_EOL;

    string_buffer_offset += script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset);

#if defined(INSANI)
    // this block is necessary because as it stands, onscripter will not properly advance the string buffer offset past the @ symbol when in 1 byte mode, causing an infinite stall.
    if(legacy_english_mode)
    {
        if(script_h.getStringBuffer()[ string_buffer_offset + 1 ] == '@') string_buffer_offset += 2;
    }
#endif

    if ( (skip_mode & (SKIP_NORMAL | SKIP_TO_EOP) || ctrl_pressed_status) && !textgosub_label ){
        clickstr_state = CLICK_NONE;
        if ( out_text ){
            drawChar( out_text, &sentence_font, false, true, accumulation_surface, &text_info );
        }
        else{ // called on '@'
            flush(refreshMode());
        }
        num_chars_in_sentence = 0;

        event_mode = IDLE_EVENT_MODE;
        if ( waitEvent(0) )
        {
            return false;
        }
    }
    else{
        if ( out_text ){
            drawChar( out_text, &sentence_font, true, true, accumulation_surface, &text_info );
            num_chars_in_sentence++;
        }

        while( (!(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
                script_h.getStringBuffer()[ string_buffer_offset ] == ' ') ||
               script_h.getStringBuffer()[ string_buffer_offset ] == '\t' ) string_buffer_offset ++;

        if (textgosub_label){
            saveon_flag = false;

            textgosub_clickstr_state = CLICK_WAIT;
            if (script_h.getStringBuffer()[string_buffer_offset] == 0x0)
                textgosub_clickstr_state |= CLICK_EOL;
            gosubReal(textgosub_label, script_h.getWait(), true);

            event_mode = IDLE_EVENT_MODE;
            waitEvent(0);

            return false;
        }

        // if this is the end of the line, pretext becomes enabled
        if (script_h.getStringBuffer()[string_buffer_offset] == 0x0)
            line_enter_status = 0;

        clickstr_state = CLICK_WAIT;
        if (doClickEnd())
        {
            return false;
        }

        clickstr_state = CLICK_NONE;

        if (pagetag_flag) processEOT();
        page_enter_status = 0;
    }

    return true;
}

bool ONScripter::clickNewPage( char *out_text )
{
    if ( out_text ){
        drawChar( out_text, &sentence_font, true, true, accumulation_surface, &text_info );
        num_chars_in_sentence++;
    }

    flush( REFRESH_NONE_MODE );
    skip_mode &= ~SKIP_TO_EOL;

    string_buffer_offset += script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset);

#if defined(INSANI)
    // this block is necessary because as it stands, onscripter will not properly advance the string buffer offset past the \ symbol when in 1 byte mode, causing an infinite stall.
    if(legacy_english_mode)
    {
        if(script_h.getStringBuffer()[ string_buffer_offset + 1 ] == '\\') string_buffer_offset += 2;
    }
#endif

    if ( (skip_mode & SKIP_NORMAL || ctrl_pressed_status) && !textgosub_label  ){
        num_chars_in_sentence = 0;
        clickstr_state = CLICK_NEWPAGE;

        event_mode = IDLE_EVENT_MODE;
        if (waitEvent(0)) return false;
    }
    else{
        while( (!(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
               script_h.getStringBuffer()[ string_buffer_offset ] == ' ') ||
               script_h.getStringBuffer()[ string_buffer_offset ] == '\t' ) string_buffer_offset ++;

        if (textgosub_label){
            saveon_flag = false;

            textgosub_clickstr_state = CLICK_NEWPAGE;
            gosubReal(textgosub_label, script_h.getWait(), true);

            event_mode = IDLE_EVENT_MODE;
            waitEvent(0);

            return false;
        }

        // if this is the end of the line, pretext becomes enabled
        if (script_h.getStringBuffer()[string_buffer_offset] == 0x0)
            line_enter_status = 0;

        clickstr_state = CLICK_NEWPAGE;
        if (doClickEnd()) return false;
    }

    newPage();
    clickstr_state = CLICK_NONE;

    return true;
}

void ONScripter::startRuby(const char *buf, FontInfo &info)
{
    ruby_struct.stage = RubyStruct::BODY;
    ruby_font = info;
    ruby_font.ttf_font[0] = NULL;
    ruby_font.ttf_font[1] = NULL;
    if ( ruby_struct.font_size_xy[0] != -1 )
        ruby_font.font_size_xy[0] = ruby_struct.font_size_xy[0];
    else
        ruby_font.font_size_xy[0] = info.font_size_xy[0]/2;
    if ( ruby_struct.font_size_xy[1] != -1 )
        ruby_font.font_size_xy[1] = ruby_struct.font_size_xy[1];
    else
        ruby_font.font_size_xy[1] = info.font_size_xy[1]/2;
                
    ruby_struct.body_count = 0;
    ruby_struct.ruby_count = 0;

    while(1){
        if ( *buf == '/' ){
            ruby_struct.stage = RubyStruct::RUBY;
            ruby_struct.ruby_start = buf+1;
            buf++;
        }
        else if ( *buf == ')' || *buf == '\0' ){
            break;
        }
        else{
            int n = script_h.enc.getBytes(*buf);
            if (ruby_struct.stage == RubyStruct::BODY)
                ruby_struct.body_count += (n == 1)?1:2;
            else if (ruby_struct.stage == RubyStruct::RUBY)
                ruby_struct.ruby_count += (n == 1)?1:2;
            buf += n;
        }
    }
    ruby_struct.ruby_end = buf;
    ruby_struct.stage = RubyStruct::BODY;
    ruby_struct.margin = ruby_font.initRuby(info, ruby_struct.body_count/2, ruby_struct.ruby_count/2);
}

void ONScripter::endRuby(bool flush_flag, bool lookback_flag, SDL_Surface *surface, AnimationInfo *cache_info)
{
    char out_text[4]= {};
    if (sentence_font.rubyon_flag){
        ruby_font.clear();
        const char *buf = ruby_struct.ruby_start;
        while(buf < ruby_struct.ruby_end){
            int n = 2;
            if (script_h.enc.getEncoding() == Encoding::CODE_UTF8)
                n = script_h.enc.getBytes(buf[0]);
            for (int i=0; i<n; i++)
                out_text[i] = buf[i];
            drawChar(out_text, &ruby_font, flush_flag, lookback_flag, surface, cache_info);
            buf += n;
        }
    }
    ruby_struct.stage = RubyStruct::NONE;
}

#if defined(INSANI)
/*
 * int u8strlen(const char *s)
 * --
 * A simple function to grab the number of glyphs in a given UTF8-encoded string.
 * Works just like standard strlen.  Necessary for the initial version of the 
 * insani legacy linewrap algorithm with UTF8-encoded 0.utf.
 */
int ONScripter::u8strlen(const char *s)
{
    int len = 0;
    while (*s) {
        if ((*s & 0xC0) != 0x80) len++;
        s++;
    }
    return len;
}

/*
 * int strpxlen(const char *buf, FontInfo *fi)
 * --
 * A function to return the pixels taken up by a given string.  A critical part
 * of the insani linewrap algorithm for all non-CJK modes.
 */
int ONScripter::strpxlen(const char *buf, FontInfo *fi)
{
    openFont(fi);
    
    int w = 0;
    while (buf[0] != '\0')
    {
        int n = script_h.enc.getBytes(buf[0]);
        unsigned short unicode = script_h.enc.getUTF16(buf);
        
        int minx, maxx, miny, maxy, advanced;
        TTF_GlyphMetrics((TTF_Font*)fi->ttf_font[0], unicode,
                         &minx, &maxx, &miny, &maxy, &advanced);
        
        w += advanced + fi->pitch_xy[0] - fi->font_size_xy[0];
        buf += n;
    }
    w -= fi->pitch_xy[0] - fi->font_size_xy[0];

    return w;
}
#endif

int ONScripter::textCommand()
{
    if (line_enter_status <= 1 && (!pretextgosub_label || saveon_flag) && internal_saveon_flag){
        storeSaveFile();
        internal_saveon_flag = false;
    }

    char *buf = script_h.getStringBuffer();

    bool tag_flag = true;
    unsigned short unicode = script_h.enc.getUTF16("y", Encoding::CODE_CP932);
    int n = script_h.enc.getBytes(buf[string_buffer_offset]);
    if (buf[string_buffer_offset] == '[')
        string_buffer_offset++;
    else if (zenkakko_flag &&
             script_h.enc.getUTF16(buf + string_buffer_offset) == unicode)
        string_buffer_offset += n;
    else
        tag_flag = false;

    int start_offset = string_buffer_offset;
    int end_offset = start_offset;
    while (tag_flag && buf[string_buffer_offset]){
        unsigned short unicode = script_h.enc.getUTF16("z", Encoding::CODE_CP932);
        int n = script_h.enc.getBytes(buf[string_buffer_offset]);
        if (zenkakko_flag &&
            script_h.enc.getUTF16(buf + string_buffer_offset) == unicode){
            end_offset = string_buffer_offset;
            string_buffer_offset += n;
            break;
        }
        else if (buf[string_buffer_offset] == ']'){
            end_offset = string_buffer_offset;
            string_buffer_offset++;
            break;
        }
        else
            string_buffer_offset += n;
    }

    if (pretextgosub_label &&
        !last_nest_info->pretextgosub_flag &&
        (!pagetag_flag || page_enter_status == 0) &&
        line_enter_status == 0){

        if (current_page->tag) delete[] current_page->tag;
        if (end_offset > start_offset){
            int len = end_offset - start_offset;
            current_page->tag = new char[len+1];
            memcpy(current_page->tag, buf + start_offset, len);
            current_page->tag[len] = 0;
        }
        else{
            current_page->tag = NULL;
        }

        saveon_flag = false;
        pretext_buf = script_h.getCurrent();
        gosubReal(pretextgosub_label, script_h.getCurrent(), false, true);
        line_enter_status = 1;

        return RET_CONTINUE;
    }

#if defined(INSANI)
    // set english_mode and legacy_english mode here
    int english_mode_check = script_h.getEnglishMode();
    if(english_mode_check == 1)
    {
        legacy_english_mode = false;
        english_mode = true;
    }
    else if(english_mode_check == 2)
    {
        legacy_english_mode = true;
        english_mode = true;
    }
    else if(english_mode_check == 0)
    {
        legacy_english_mode = false;
        english_mode = false;
    }

/*
    // below is the first iteration of the insani linewrap algorithm; now here just in case
    if(legacy_english_mode)
    {
        // English monospaced line wrapping algorithm begins here
        int line_length = sentence_font.num_xy[0] * 2;
        char *original_text = script_h.getStringBuffer();
        int current_line_length = 0;

        prev_skip_newline_mode = skip_newline_mode;
        skip_newline_mode = script_h.getSkipNewlineMode();
        if(prev_skip_newline_mode && original_text[strlen(original_text) - 1] == '\\') skip_newline_mode = true;

        //printf("skip_newline_mode: %d\n", skip_newline_mode);
        if(!skip_newline_mode) skip_newline_offset = 0;
        else if(skip_newline_mode) current_line_length += skip_newline_offset;
        //printf("skip_newline_offset: %d\n", skip_newline_offset);

        // in ScriptHandler.cpp: #define STRING_BUFFER_LENGTH 4096 -- this is the max string buffer length ONScripter supports.
        char *temp_text = (char *) malloc(4096 * sizeof(char));

        int original_text_length = u8strlen(original_text);
        int original_text_bytes = strlen(original_text);
        bool char_is_token = true;
        int char_is_token_counter;
        strcpy(temp_text, original_text);

        // now get the first two words in the script
        char *current_word = strtok(temp_text, " ");
        char *next_word = strtok(NULL, " ");

        // check for padding spaces in the first word
        if(&current_word[0] != &temp_text[0])
        {
            int space_counter = 0;
            for(int i = 0; original_text[i] == ' '; i++)
            {
                temp_text[i] = ' ';
                space_counter++;
            }
            if(current_line_length + u8strlen(current_word) + space_counter <= line_length) current_word = temp_text;
        }

        // now parse special cases
        if(u8strlen(current_word) >= line_length)
        {
            // ... then there's no point in word-wrapping at all, as the word that the writer/translator put in is longer than the limit anyway, so do nothing
        }
        else if(original_text_length + current_line_length <= line_length)
        {
            // ... the entire text fits on one display line anyway, so just calculate the new skip_newline_offset
            if(skip_newline_mode)
            {
                // remove all special ending tokens (\, @, /) from the character count
                char_is_token = true;
                char_is_token_counter = original_text_bytes - 1;
                while(char_is_token)
                {
                    if(original_text[char_is_token_counter] == '\\' ||
                       original_text[char_is_token_counter] == '@' ||
                       original_text[char_is_token_counter] == '/')
                    {
                        original_text_length--;
                        char_is_token_counter--;
                    }
                    else char_is_token = false;
                }

                // then set the skip_newline_offset to the corrected text length
                skip_newline_offset += original_text_length;

            }
            else skip_newline_offset = 0;
        }
        else
        {
            // first attempt to iterate manually on current_word or next_word
            if(current_line_length + u8strlen(current_word) + 1 <= line_length)
            {
                strcpy(original_text, current_word);
                current_word = next_word;
                next_word = strtok(NULL, " ");

                // we capture this in a variable because we need to reset this to 0 whenever we line break
                current_line_length += u8strlen(original_text);
            }
            else
            {
                // the only time the above won't work is if we had a / before and need to word wrap on the first word
                strcpy(original_text, "");
                //current_line_length++;
                while(current_line_length < line_length)
                {
                    strcat(original_text, " ");
                    current_line_length++;
                }
                skip_newline_offset = 0;

                // now we've begun the new line, and we set the current line length to the current word
                strcat(original_text, current_word);
                current_line_length = u8strlen(current_word);

                // and finally, iterate current_word and next_word
                current_word = next_word;
                next_word = strtok(NULL, " ");
            }

            // ...our setup is complete and now we can loop and line break
            while(current_word != NULL)
            {
                // Word wrap special cases.  Not appropriate to start lines with
                // "--" or "...", so always stick these with the last word.  Also,
                // if you detect a double space, preserve that double space.
                int null_index = strlen(current_word);
                null_index = null_index + 2;

                if(next_word != NULL &&
                (strcmp(next_word, "-") == 0 ||
                 strcmp(next_word, "--") == 0 || 
                 (next_word[0] == '.' &&
                  next_word[1] == '.' &&
                  next_word[2] == '.')))
                {
                    // attempt to replace the null character that terminates current_word with a space; should have the effect of concatenating the two strings together
                    null_index = strlen(current_word);
                    current_word[null_index] = ' ';

                    next_word = strtok(NULL, " ");
                }
                else if(next_word != NULL && 
                        (&current_word[null_index] == &next_word[0]))
                {
                    // replace the null character that terminates current_word with a space; this will not, unlike above, concatenate current_word and next_word together because there is another space
                    null_index = null_index - 2;
                    current_word[null_index] = ' ';
                    current_word[null_index + 1] = '\0';
                }

                if(current_line_length + u8strlen(current_word) + 1 <= line_length)
                {
                    // ... the length of the wrapped text and the current word are still less than the max line length, so ...
                    // ... add a space, then the current word.
                    strcat(original_text, " ");
                    strcat(original_text, current_word);

                    current_line_length = current_line_length + u8strlen(current_word) + 1;
                }
                else
                {
                    //printf("current_line_length: %d\n", current_line_length);
                    // we are in line break mode

                    // skip_newline_offset reset to 0
                    skip_newline_offset = 0;

                    // first fill the line with spaces all the way up to the line length limit
                    //while(current_line_length < line_length)
                    //{
                    //    strcat(original_text, " ");
                    //    current_line_length++;
                    //}
                    strcat(original_text, "\n");

                    // now we've begun the new line, and we set the current line length to the current word
                    strcat(original_text, current_word);
                    current_line_length = u8strlen(current_word);

                }

                //printf("%s\n", original_text);

                // these are the last things that should happen before we loop
                // remove all special ending tokens (\, @, /) from the character count
                char_is_token = true;
                char_is_token_counter = u8strlen(original_text) - 1;
                while(char_is_token)
                {
                    if(original_text[char_is_token_counter] == '\\' ||
                       original_text[char_is_token_counter] == '@' ||
                       original_text[char_is_token_counter] == '/')
                    {
                        current_line_length--;
                        char_is_token_counter--;
                    }
                    else char_is_token = false;
                }

                // then set the skip_newline_offset to the corrected character count
                skip_newline_offset = current_line_length;

                //printf("%s %d %d\n", current_word, u8strlen(current_word), current_line_length);

                // and finally iterate through the next set of words
                current_word = next_word;
                next_word = strtok(NULL, " ");
            }
        }
        free(temp_text);
    }
*/

    if(english_mode)
    {
        // English pixel-based line wrapping algorithm begins here
        int line_length = sentence_font.num_xy[0] * sentence_font.pitch_xy[0];
        char *original_text = script_h.getStringBuffer();
        int current_line_length = 0;

        prev_skip_newline_mode = skip_newline_mode;
        skip_newline_mode = script_h.getSkipNewlineMode();
        if(skip_newline_mode) { printf("in skip newline mode\n"); printf("offset: %d\n", skip_newline_offset); }
        if(prev_skip_newline_mode && original_text[strlen(original_text) - 1] == '/') skip_newline_mode = true;

        if(!prev_skip_newline_mode) skip_newline_offset = 0;
        else if(prev_skip_newline_mode) current_line_length += skip_newline_offset;

        // in ScriptHandler.cpp: #define STRING_BUFFER_LENGTH 4096 -- this is the max string buffer length ONScripter supports.
        char *temp_text = (char *) malloc(4096 * sizeof(char));

        int original_text_length = strpxlen(original_text, &sentence_font);
        int original_text_bytes = strlen(original_text);
        bool char_is_token = true;
        int char_is_token_counter;
        strcpy(temp_text, original_text);

        // now get the first two words in the script
        char *current_word = strtok(temp_text, " ");
        char *next_word = strtok(NULL, " ");

        // check for padding spaces in the first word
        if(&current_word[0] != &temp_text[0])
        {
            int space_counter = 0;
            for(int i = 0; original_text[i] == ' '; i++)
            {
                temp_text[i] = ' ';
                space_counter += strpxlen(" ", &sentence_font);
            }
            if(current_line_length + strpxlen(current_word, &sentence_font) + space_counter <= line_length) current_word = temp_text;
        }

        // now parse special cases
        if(strpxlen(current_word, &sentence_font) >= line_length)
        {
            // ... then there's no point in word-wrapping at all, as the word that the writer/translator put in is longer than the limit anyway, so do nothing
        }
        else if(original_text_length + current_line_length <= line_length)
        {
            // ... the entire text fits on one display line anyway, so just calculate the new skip_newline_offset
            if(skip_newline_mode)
            {
                // remove all special ending tokens (\, @, /) from the character count
                char_is_token = true;
                char_is_token_counter = original_text_bytes - 1;
                while(char_is_token)
                {
                    if(original_text[char_is_token_counter] == '\\' ||
                       original_text[char_is_token_counter] == '@' ||
                       original_text[char_is_token_counter] == '/')
                    {
                        if(original_text[char_is_token_counter] == '\\') original_text_length -= strpxlen("\\", &sentence_font);
                        else if(original_text[char_is_token_counter] == '@') original_text_length -= strpxlen("@", &sentence_font);
                        else if(original_text[char_is_token_counter] == '/') original_text_length -= strpxlen("/", &sentence_font);
                        char_is_token_counter--;
                    }
                    else char_is_token = false;
                }

                // then set the skip_newline_offset to the corrected text length
                skip_newline_offset += original_text_length;

            }
            else skip_newline_offset = 0;
        }
        else
        {
            // first attempt to iterate manually on current_word or next_word
            if(current_line_length + strpxlen(current_word, &sentence_font) + strpxlen(" ", &sentence_font) <= line_length)
            {
                strcpy(original_text, current_word);
                current_word = next_word;
                next_word = strtok(NULL, " ");

                // we capture this in a variable because we need to reset this to 0 whenever we line break
                current_line_length += strpxlen(original_text, &sentence_font);
                if(skip_newline_mode) skip_newline_offset = current_line_length;
                else skip_newline_offset = 0;
            }
            else
            {
                // the only time the above won't work is if we had a / before and need to word wrap on the first word
                strcpy(original_text, "");
                //current_line_length++;
                while(current_line_length < line_length)
                {
                    strcat(original_text, " ");
                    current_line_length += strpxlen(" ", &sentence_font);
                }

                // now we've begun the new line, and we set the current line length to the current word
                strcat(original_text, current_word);
                current_line_length = strpxlen(current_word, &sentence_font);
                if(skip_newline_mode) skip_newline_offset = current_line_length;
                else skip_newline_offset = 0;

                // and finally, iterate current_word and next_word
                current_word = next_word;
                next_word = strtok(NULL, " ");
            }

            // ...our setup is complete and now we can loop and line break
            while(current_word != NULL)
            {
                /*
                *
                * Word wrap special cases.  Not appropriate to start lines with
                * "--" or "...", so always stick these with the last word.  Also,
                * if you detect a double space, preserve that double space.
                * 
                * Finally, if you detect an emdash or a horizontal bar, also make
                * it nonbreaking.
                * 
                */
                int null_index = strlen(current_word);
                null_index = null_index + 2;

                if(next_word != NULL &&
                   ((strlen(current_word) == 4 && (unsigned char) current_word[0] == 0xe2 && (unsigned char) current_word[1] == 0x80 && (unsigned char) current_word[2] == 0x94) ||
                    (strlen(current_word) == 4 && (unsigned char) current_word[0] == 0xe2 && (unsigned char) current_word[1] == 0x80 && (unsigned char) current_word[2] == 0x95) ||
                    (strlen(current_word) == 3 && (unsigned char) current_word[0] == 0xc2 && (unsigned char) current_word[1] == 0xab) ||
                    (strlen(current_word) == 4 && (unsigned char) current_word[0] == 0xe2 && (unsigned char) current_word[1] == 0x80 && (unsigned char) current_word[2] == 0xb9) ||
                    (strlen(next_word) == 3 && (unsigned char) next_word[0] == 0xc2 && (unsigned char) next_word[1] == 0xbb) ||
                    (strlen(next_word) == 4 && (unsigned char) next_word[0] == 0xe2 && (unsigned char) next_word[1] == 0x80 && (unsigned char) next_word[2] == 0xba) ||
                    strcmp(next_word, "-") == 0 ||
                    strcmp(next_word, "--") == 0 || 
                    (next_word[0] == '.' &&
                     next_word[1] == '.' &&
                     next_word[2] == '.')))
                {
                    // attempt to replace the null character that terminates current_word with a space; should have the effect of concatenating the two strings together
                    null_index = strlen(current_word);
                    current_word[null_index] = ' ';

                    next_word = strtok(NULL, " ");
                }
                else if(next_word != NULL && 
                        (&current_word[null_index] == &next_word[0]))
                {
                    // replace the null character that terminates current_word with a space; this will not, unlike above, concatenate current_word and next_word together because there is another space
                    null_index = null_index - 2;
                    current_word[null_index] = ' ';
                    current_word[null_index + 1] = '\0';
                }

                if(current_line_length + strpxlen(current_word, &sentence_font) + strpxlen(" ", &sentence_font) <= line_length)
                {
                    // ... the length of the wrapped text and the current word are still less than the max line length, so ...
                    // ... add a space, then the current word.
                    strcat(original_text, " ");
                    strcat(original_text, current_word);

                    current_line_length = current_line_length + strpxlen(current_word, &sentence_font) + strpxlen(" ", &sentence_font);
                }
                else
                {
                    // we are in line break mode

                    // skip_newline_offset reset to 0
                    skip_newline_offset = 0;

                    // add a newline marker for later linebreaking in processText()
                    strcat(original_text, "\n");

                    // now we've begun the new line, and we set the current line length to the current word
                    strcat(original_text, current_word);
                    current_line_length = strpxlen(current_word, &sentence_font);

                }

                //printf("%s\n", original_text);

                // these are the last things that should happen before we loop
                // remove all special ending tokens (\, @, /) from the character count
                char_is_token = true;
                char_is_token_counter = u8strlen(original_text) - 1;
                while(char_is_token)
                {
                    if(original_text[char_is_token_counter] == '\\' ||
                       original_text[char_is_token_counter] == '@' ||
                       original_text[char_is_token_counter] == '/')
                    {
                        if(original_text[char_is_token_counter] == '\\') original_text_length -= strpxlen("\\", &sentence_font);
                        else if(original_text[char_is_token_counter] == '@') original_text_length -= strpxlen("@", &sentence_font);
                        else if(original_text[char_is_token_counter] == '/') original_text_length -= strpxlen("/", &sentence_font);
                        char_is_token_counter--;
                    }
                    else char_is_token = false;
                }

                // then set the skip_newline_offset to the corrected character count
                skip_newline_offset = current_line_length;

                //printf("%s %d %d\n", current_word, u8strlen(current_word), current_line_length);

                // and finally iterate through the next set of words
                current_word = next_word;
                next_word = strtok(NULL, " ");
            }
        }
        free(temp_text);
    }

#endif

    enterTextDisplayMode();

#ifdef USE_LUA
    if (lua_handler.isCallbackEnabled(LUAHandler::LUA_TEXT))
    {
        if (lua_handler.callFunction(true, "text"))
            errorAndExit( lua_handler.error_str );
        processEOT();
    }
    else
#endif
    while(processText());

    return RET_CONTINUE;
}

bool ONScripter::checkLineBreak(const char *buf, FontInfo *fi)
{
    if (!is_kinsoku) return false;
    
    // check start kinsoku
    int n = script_h.enc.getBytes(buf[0]);
    if (isStartKinsoku(buf+n) ||
        (buf[n]=='_' && isStartKinsoku(buf+n+1))){
        const char *buf2 = buf + n;
        if (buf2[0] == '_') buf2++;
        int i = 2;
        while (!fi->isEndOfLine(i)){
            n = script_h.enc.getBytes(buf2[0]);
            if      (buf2[n] == 0x0a || buf2[n] == 0) break;
            else if (script_h.enc.getBytes(buf2[n]) == 1) buf2++;
            else if (isStartKinsoku(buf2 + n)){
                i += 2;
                buf2 += n;
            }
            else break;
        }

        if (fi->isEndOfLine(i)) return true;
    }
        
    // check end kinsoku
    if (isEndKinsoku(buf)){
        const char *buf2 = buf + n;
        int i = 2;
        while (!fi->isEndOfLine(i)){
            n = script_h.enc.getBytes(buf2[0]);
            if      (buf2[n] == 0x0a || buf2[n] == 0) break;
            else if (script_h.enc.getBytes(buf2[n]) == 1) buf2++;
            else if (isEndKinsoku(buf2 + n)){
                i += 2;
                buf2 += n;
            }
            else break;
        }

        if (fi->isEndOfLine(i)) return true;
    }

    return false;
}

bool ONScripter::checkLigatureLineBreak(const char *buf, FontInfo *fi)
{
    if (script_h.current_language != 0) return false;

    openFont(fi);
    
    int w = 0;
    while (buf[0] != ' ' && buf[0] != 0x0a && buf[0] != 0 &&
           buf[0] != script_h.enc.getTextMarker()){
        int n = script_h.enc.getBytes(buf[0]);
        unsigned short unicode = script_h.enc.getUTF16(buf);
        
        int minx, maxx, miny, maxy, advanced;
        TTF_GlyphMetrics((TTF_Font*)fi->ttf_font[0], unicode,
                         &minx, &maxx, &miny, &maxy, &advanced);
        
        w += advanced + fi->pitch_xy[0] - fi->font_size_xy[0];
        buf += n;
    }
    w -= fi->pitch_xy[0] - fi->font_size_xy[0];

    return fi->isEndOfLine(w * 2.0 / fi->pitch_xy[0]);
}

void ONScripter::processEOT()
{
    if ( skip_mode & SKIP_TO_EOL ){
        flush( refreshMode() );
        skip_mode &= ~SKIP_TO_EOL;
    }

    if (!sentence_font.isLineEmpty() && !new_line_skip_flag){
        // if sentence_font.isLineEmpty() is true, newPage() might be already issued
        if (!sentence_font.isEndOfLine()) current_page->add( 0x0a );
        sentence_font.newLine();
    }

    if (!new_line_skip_flag && !pagetag_flag && line_enter_status == 2) line_enter_status = 0;
}

bool ONScripter::processText()
{
    //printf("textCommand %c %d %d %d\n", script_h.getStringBuffer()[ string_buffer_offset ], string_buffer_offset, event_mode, line_enter_status);
    char out_text[4]= {};

    //printf("*** textCommand %d (%d,%d)\n", string_buffer_offset, sentence_font.xy[0], sentence_font.xy[1]);

    while( (!(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
            script_h.getStringBuffer()[ string_buffer_offset ] == ' ') ||
           script_h.getStringBuffer()[ string_buffer_offset ] == '\t' ) string_buffer_offset ++;

    if (script_h.getStringBuffer()[string_buffer_offset] == 0x00){
        processEOT();
        return false;
    }

    line_enter_status = 2;
    if (pagetag_flag) page_enter_status = 1;

    new_line_skip_flag = false;

    char ch = script_h.getStringBuffer()[string_buffer_offset];

    int n = script_h.enc.getBytes(ch);
    if (n >= 2){
        /* ---------------------------------------- */
        /* Kinsoku process */
        if (checkLineBreak(script_h.getStringBuffer() + string_buffer_offset, &sentence_font)){
            sentence_font.newLine();
            for (int i=0 ; i<indent_offset ; i++){
                if (script_h.enc.getEncoding() == Encoding::CODE_CP932){
                    current_page->add(0x81);
                    current_page->add(0x40);
                }
                else{
                    current_page->add(0xe3);
                    current_page->add(0x80);
                    current_page->add(0x80);
                }
                sentence_font.advanceCharInHankaku(2);
            }
        }

        for (int i=0; i<n; i++)
            out_text[i] = script_h.getStringBuffer()[string_buffer_offset+i];

        if (script_h.checkClickstr(&script_h.getStringBuffer()[string_buffer_offset]) > 0){
            if (sentence_font.getRemainingLine() <= clickstr_line)
                return clickNewPage( out_text );
            else
                return clickWait( out_text );
        }
        else{
            clickstr_state = CLICK_NONE;
        }
        
        if ( skip_mode || ctrl_pressed_status ){
            drawChar( out_text, &sentence_font, false, true, accumulation_surface, &text_info );
        }
        else{
            drawChar( out_text, &sentence_font, true, true, accumulation_surface, &text_info );

            event_mode = WAIT_TIMER_MODE | WAIT_INPUT_MODE;
            if ( sentence_font.wait_time == -1 )
                waitEvent( default_text_speed[text_speed_no] );
            else
                waitEvent( sentence_font.wait_time );
        }
        
        num_chars_in_sentence++;
        string_buffer_offset += n;

        return true;
    }
    else if ( ch == '@' ){ // wait for click
        return clickWait( NULL );
    }
    else if ( ch == '\\' ){ // new page
        return clickNewPage( NULL );
    }
    else if ( ch == '_' ){ // Ignore an immediate click wait
        string_buffer_offset++;

        int matched_len = script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset, true);
        if (matched_len > 0){
            out_text[0] = script_h.getStringBuffer()[string_buffer_offset];
            if (out_text[0] != '@' && out_text[0] != '\\'){
                for (int i=1; i<matched_len; i++)
                    out_text[i] = script_h.getStringBuffer()[string_buffer_offset+i];
                bool flush_flag = true;
                if ( skip_mode || ctrl_pressed_status ) flush_flag = false;
                drawChar( out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info );
            }
            string_buffer_offset += matched_len;
        }
        
        return true;
    }
#if defined(INSANI)
    else if ( ch == '\n' && english_mode ){
        if(sentence_font.is_newline_accepted == false) sentence_font.is_newline_accepted = true;
        sentence_font.newLine();
        string_buffer_offset++;
        current_page->add('\n');
        return true;
    }
    else if ( ch == '!' && english_mode ){
        if( script_h.getStringBuffer()[ string_buffer_offset + 1 ] == 's' ||
            script_h.getStringBuffer()[ string_buffer_offset + 1 ] == 'w')
        {
            string_buffer_offset++;
            if ( script_h.getStringBuffer()[ string_buffer_offset ] == 's' ){
                string_buffer_offset++;
                if ( script_h.getStringBuffer()[ string_buffer_offset ] == 'd' ){
                    sentence_font.wait_time = -1;
                    string_buffer_offset++;
                }
                else{
                    int t = 0;
                    while( script_h.getStringBuffer()[ string_buffer_offset ] >= '0' &&
                        script_h.getStringBuffer()[ string_buffer_offset ] <= '9' ){
                        t = t*10 + script_h.getStringBuffer()[ string_buffer_offset ] - '0';
                        string_buffer_offset++;
                    }
                    sentence_font.wait_time = t;
                    while (script_h.getStringBuffer()[ string_buffer_offset ] == ' ' ||
                        script_h.getStringBuffer()[ string_buffer_offset ] == '\t') string_buffer_offset++;
                }
            }
            else if ( script_h.getStringBuffer()[ string_buffer_offset ] == 'w' ||
                    script_h.getStringBuffer()[ string_buffer_offset ] == 'd' ){
                bool flag = false;
                if ( script_h.getStringBuffer()[ string_buffer_offset ] == 'd' ) flag = true;
                string_buffer_offset++;
                int t = 0;
                while( script_h.getStringBuffer()[ string_buffer_offset ] >= '0' &&
                    script_h.getStringBuffer()[ string_buffer_offset ] <= '9' ){
                    t = t*10 + script_h.getStringBuffer()[ string_buffer_offset ] - '0';
                    string_buffer_offset++;
                }
                while (script_h.getStringBuffer()[ string_buffer_offset ] == ' ' ||
                    script_h.getStringBuffer()[ string_buffer_offset ] == '\t') string_buffer_offset++;
                if (!skip_mode && !ctrl_pressed_status){
                    event_mode = WAIT_TIMER_MODE;
                    if (flag) event_mode |= WAIT_INPUT_MODE;
                    waitEvent(t);
                }
            }
            return true;
        }
        else
        {
            out_text[0] = script_h.getStringBuffer()[string_buffer_offset];
            drawChar( out_text, &sentence_font, true, true, accumulation_surface, &text_info );
            string_buffer_offset++;
            num_chars_in_sentence++;
            return true;
        }
    }
#endif
    else if ( ch == '!' && !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) ){
        string_buffer_offset++;
        if ( script_h.getStringBuffer()[ string_buffer_offset ] == 's' ){
            string_buffer_offset++;
            if ( script_h.getStringBuffer()[ string_buffer_offset ] == 'd' ){
                sentence_font.wait_time = -1;
                string_buffer_offset++;
            }
            else{
                int t = 0;
                while( script_h.getStringBuffer()[ string_buffer_offset ] >= '0' &&
                       script_h.getStringBuffer()[ string_buffer_offset ] <= '9' ){
                    t = t*10 + script_h.getStringBuffer()[ string_buffer_offset ] - '0';
                    string_buffer_offset++;
                }
                sentence_font.wait_time = t;
                while (script_h.getStringBuffer()[ string_buffer_offset ] == ' ' ||
                       script_h.getStringBuffer()[ string_buffer_offset ] == '\t') string_buffer_offset++;
            }
        }
        else if ( script_h.getStringBuffer()[ string_buffer_offset ] == 'w' ||
                  script_h.getStringBuffer()[ string_buffer_offset ] == 'd' ){
            bool flag = false;
            if ( script_h.getStringBuffer()[ string_buffer_offset ] == 'd' ) flag = true;
            string_buffer_offset++;
            int t = 0;
            while( script_h.getStringBuffer()[ string_buffer_offset ] >= '0' &&
                   script_h.getStringBuffer()[ string_buffer_offset ] <= '9' ){
                t = t*10 + script_h.getStringBuffer()[ string_buffer_offset ] - '0';
                string_buffer_offset++;
            }
            while (script_h.getStringBuffer()[ string_buffer_offset ] == ' ' ||
                   script_h.getStringBuffer()[ string_buffer_offset ] == '\t') string_buffer_offset++;
            if (!skip_mode && !ctrl_pressed_status){
                event_mode = WAIT_TIMER_MODE;
                if (flag) event_mode |= WAIT_INPUT_MODE;
                waitEvent(t);
            }
        }
        return true;
    }
    else if (ch == '#'){
        readColor( &sentence_font.color, script_h.getStringBuffer() + string_buffer_offset );
        readColor( &ruby_font.color, script_h.getStringBuffer() + string_buffer_offset );
        string_buffer_offset += 7;
        return true;
    }
    else if ( ch == '(' && 
              (!english_mode ||
               !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR)) ){
        current_page->add('(');
        startRuby( script_h.getStringBuffer() + string_buffer_offset + 1, sentence_font );
        
        string_buffer_offset++;
        return true;
    }
    else if ( ch == '/' && !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) ){
        if ( ruby_struct.stage == RubyStruct::BODY ){
            current_page->add('/');
            sentence_font.addLineOffset(ruby_struct.margin);
            string_buffer_offset = ruby_struct.ruby_end - script_h.getStringBuffer();
            if (*ruby_struct.ruby_end == ')'){
                if ( skip_mode || ctrl_pressed_status )
                    endRuby(false, true, accumulation_surface, &text_info);
                else
                    endRuby(true, true, accumulation_surface, &text_info);
                current_page->add(')');
                string_buffer_offset++;
            }

            return true;
        }
        else{ // skip new line
            new_line_skip_flag = true;
            string_buffer_offset++;
            if (script_h.getStringBuffer()[string_buffer_offset] != 0x00)
                errorAndExit( "'new line' must follow '/'." );
            return true; // skip the following eol
        }
    }
    else if ( ch == ')' && !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
              ruby_struct.stage == RubyStruct::BODY ){
        current_page->add(')');
        string_buffer_offset++;
        ruby_struct.stage = RubyStruct::NONE;
        return true;
    }
    else if ( ch == '<' && 
              (!english_mode ||
               !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR)) ){
        current_page->add('<');
        string_buffer_offset++;
        int no = 0;
        while(script_h.getStringBuffer()[string_buffer_offset]>='0' &&
              script_h.getStringBuffer()[string_buffer_offset]<='9'){
            current_page->add(script_h.getStringBuffer()[string_buffer_offset]);
            no=no*10+script_h.getStringBuffer()[string_buffer_offset++]-'0';
        }
        in_textbtn_flag = true;
        return true;
    }
    else if ( ch == '>' && in_textbtn_flag &&
              (!english_mode ||
               !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR)) ){
        current_page->add('>');
        string_buffer_offset++;
        in_textbtn_flag = false;
        return true;
    }
    else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
             ch == script_h.enc.getTextMarker()){
        int status = script_h.getEndStatus();
        if (status & ScriptHandler::END_1BYTE_CHAR)
            script_h.setEndStatus(status & ~ScriptHandler::END_1BYTE_CHAR, true);
        else
            script_h.setEndStatus(ScriptHandler::END_1BYTE_CHAR);
        string_buffer_offset++;
        
        return true;
    }
    else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
             ch == '~'){
        current_page->add('~');
        while(1){
            ch = script_h.getStringBuffer()[++string_buffer_offset];
            if (ch == 0x0a || ch == 0) break;
            current_page->add(ch);
            if (ch == '~'){
                string_buffer_offset++;
                break;
            }
            if (ch == 'i'){
                openFont(&sentence_font);
                sentence_font.toggleStyle(TTF_STYLE_ITALIC);
            }
            else if (ch == 'b'){
                openFont(&sentence_font);
                sentence_font.toggleStyle(TTF_STYLE_BOLD);
            }
        }
        
        return true;
    }
    else{
        out_text[0] = ch;
        int matched_len = script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset);

        if (matched_len > 0){
            if (matched_len == 2) out_text[1] = script_h.getStringBuffer()[ string_buffer_offset + 1 ];
            if (sentence_font.getRemainingLine() <= clickstr_line)
                return clickNewPage( out_text );
            else
                return clickWait( out_text );
        }
        else if (script_h.getStringBuffer()[ string_buffer_offset + 1 ] &&
                 script_h.checkClickstr(&script_h.getStringBuffer()[string_buffer_offset+1]) == 1 &&
                 script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR){
            if ( script_h.getStringBuffer()[ string_buffer_offset + 2 ] &&
                 script_h.checkClickstr(&script_h.getStringBuffer()[string_buffer_offset+2]) > 0){
                clickstr_state = CLICK_NONE;
            }
            else if (script_h.getStringBuffer()[ string_buffer_offset + 1 ] == '@'){
                return clickWait( out_text );
            }
            else if (script_h.getStringBuffer()[ string_buffer_offset + 1 ] == '\\'){
                return clickNewPage( out_text );
            }
            else{
                out_text[1] = script_h.getStringBuffer()[ string_buffer_offset + 1 ];
                if (sentence_font.getRemainingLine() <= clickstr_line)
                    return clickNewPage( out_text );
                else
                    return clickWait( out_text );
            }
        }
        else{
            clickstr_state = CLICK_NONE;
        }
        
        bool flush_flag = true;
        if ( skip_mode || ctrl_pressed_status )
            flush_flag = false;
        if (script_h.getStringBuffer()[ string_buffer_offset + 1 ] &&
            !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
            script_h.enc.getEncoding() == Encoding::CODE_CP932){
            out_text[1] = script_h.getStringBuffer()[ string_buffer_offset + 1 ];
            drawChar(out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info);
            num_chars_in_sentence++;
        }
        else if (script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR ||
                 script_h.enc.getEncoding() == Encoding::CODE_UTF8){
            if (checkLigatureLineBreak(script_h.getStringBuffer() + string_buffer_offset, &sentence_font))
                sentence_font.newLine();
            drawChar(out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info);
            num_chars_in_sentence++;
        }
        
        if (!skip_mode && !ctrl_pressed_status){
            event_mode = WAIT_TIMER_MODE | WAIT_INPUT_MODE;
            if ( sentence_font.wait_time == -1 )
                waitEvent( default_text_speed[text_speed_no] );
            else
                waitEvent( sentence_font.wait_time );
        }

        if (script_h.getStringBuffer()[ string_buffer_offset + 1 ] &&
            !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
            script_h.enc.getEncoding() == Encoding::CODE_CP932)
            string_buffer_offset++;
        string_buffer_offset++;

        return true;
    }

    return false;
}


