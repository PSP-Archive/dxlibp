/*
 * intraFont.h
 * This file is used to display the PSP's internal font (pgf and bwfon firmware files)
 * intraFont Version 0.31 by BenHur - http://www.psp-programming.com/benhur
 *
 * Uses parts of pgeFont by InsertWittyName - http://insomniac.0x89.org
 *
 * This work is licensed under the Creative Commons Attribution-Share Alike 3.0 License.
 * See LICENSE for more details.
 *
 */

#ifndef __INTRAFONT_H__
#define __INTRAFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "intrafont_wrapper.h"

#include "libccc.h"


#define INTRAFONT_ADVANCE_H        0x00000000 //default: advance horizontaly from one char to the next
#define INTRAFONT_ADVANCE_V        0x00000100
#define INTRAFONT_ALIGN_LEFT       0x00000000 //default: left-align the text
#define INTRAFONT_ALIGN_CENTER     0x00000200
#define INTRAFONT_ALIGN_RIGHT      0x00000400
#define INTRAFONT_ALIGN_FULL       0x00000600 //full justify text to width set by intraFontSetTextWidth()
#define INTRAFONT_SCROLL_LEFT      0x00002000 //in intraFontPrintColumn if text does not fit text is scrolled to the left 
	                                          //(requires redrawing at ~60 FPS with x position returned by previous call to intraFontPrintColumn())
#define INTRAFONT_SCROLL_SEESAW    0x00002200 //in intraFontPrintColumn if text does not fit text is scrolled left and right
#define INTRAFONT_SCROLL_RIGHT     0x00002400 //in intraFontPrintColumn if text does not fit text is scrolled to the right
#define INTRAFONT_SCROLL_THROUGH   0x00002600 //in intraFontPrintColumn if text does not fit text is scrolled through (to the left)
#define INTRAFONT_WIDTH_VAR        0x00000000 //default: variable-width
#define INTRAFONT_WIDTH_FIX        0x00000800 //set your custom fixed witdh to 24 pixels: INTRAFONT_WIDTH_FIX | 24 
                                              //(max is 255, set to 0 to use default fixed width, this width will be scaled by size)
#define INTRAFONT_ACTIVE           0x00001000 //assumes the font-texture resides inside sceGuTex already, prevents unecessary reloading -> very small speed-gain									   
#define INTRAFONT_CACHE_MED        0x00000000 //default: 256x256 texture (enough to cache about 100 chars)
#define INTRAFONT_CACHE_LARGE      0x00004000 //512x512 texture(enough to cache all chars of ltn0.pgf or ... or ltn15.pgf or kr0.pgf)
#define INTRAFONT_CACHE_ASCII      0x00008000 //try to cache all ASCII chars during fontload (uses less memory and is faster to draw text, but slower to load font)
                                              //if it fails: (because the cache is too small) it will automatically switch to chache on-the-fly with a medium texture
								              //if it succeeds: (all chars and shadows fit into chache) it will free some now unneeded memory
#define INTRAFONT_CACHE_ALL        0x0000C000 //try to cache all chars during fontload (uses less memory and is faster to draw text, but slower to load font)
                                              //if it fails: (because the cache is too small) it will automatically switch to chache on-the-fly with a large texture
								        	  //if it succeeds: (all chars and shadows fit into chache) it will free some now unneeded memory
#define INTRAFONT_STRING_ASCII     (0x00010000*CCC_CP000)  //default: interpret strings as ascii text (ISO/IEC 8859-1)
#define INTRAFONT_STRING_CP437     (0x00010000*CCC_CP437)  //interpret strings as ascii text (codepage 437)
#define INTRAFONT_STRING_CP850     (0x00010000*CCC_CP850)  //interpret strings as ascii text (codepage 850)
#define INTRAFONT_STRING_CP866     (0x00010000*CCC_CP866)  //interpret strings as ascii text (codepage 866)
#define INTRAFONT_STRING_SJIS      (0x00010000*CCC_CP932)  //interpret strings as shifted-jis (used for japanese)
#define INTRAFONT_STRING_GBK       (0x00010000*CCC_CP936)  //interpret strings as GBK (used for simplified chinese)
#define INTRAFONT_STRING_KOR       (0x00010000*CCC_CP949)  //interpret strings as Korean codepage 949
#define INTRAFONT_STRING_BIG5      (0x00010000*CCC_CP950)  //interpret strings as BIG5 (used for traditional chinese)
#define INTRAFONT_STRING_CP1251    (0x00010000*CCC_CP1251) //interpret strings as ascii text (codepage windows-1251)
#define INTRAFONT_STRING_CP1252    (0x00010000*CCC_CP1252) //interpret strings as ascii text (codepage windows-1252)
#define INTRAFONT_STRING_UTF8      (0x00010000*CCC_CPUTF8) //interpret strings as UTF-8

#define FILETYPE_PGF      0x00
#define FILETYPE_BWFON    0x01
#define PGF_BMP_H_ROWS    0x01
#define PGF_BMP_V_ROWS    0x02
#define PGF_BMP_OVERLAY   0x03
#define PGF_NO_EXTRA1     0x04
#define PGF_NO_EXTRA2     0x08
#define PGF_NO_EXTRA3     0x10
#define PGF_CHARGLYPH     0x20
#define PGF_SHADOWGLYPH   0x40 //warning: this flag is not contained in the metric header flags and is only provided for simpler call to intraFontGetGlyph - ONLY check with (flags & PGF_CHARGLYPH)
#define PGF_CACHED        0x80
#define PGF_WIDTH_MASK    0x000000FF
#define PGF_OPTIONS_MASK  0x00003FFF
#define PGF_ALIGN_MASK    0x00000600
#define PGF_SCROLL_MASK   0x00002600
#define PGF_CACHE_MASK    0x0000C000
#define PGF_STRING_MASK   0x00FF0000


typedef struct {
	unsigned short x;         //in pixels
	unsigned short y;         //in pixels
	unsigned char width;      //in pixels
	unsigned char height;     //in pixels
	char left;                //in pixels
	char top;                 //in pixels
	unsigned char flags;
	unsigned short shadowID;  //to look up in shadowmap
	char advance;             //in quarterpixels
	unsigned long ptr;        //offset 
} Glyph;

typedef struct {
	unsigned short x;         //in pixels
	unsigned short y;         //in pixels
	unsigned char flags;
} GlyphBW;

typedef struct {
	unsigned short header_start;
	unsigned short header_len;
	char pgf_id[4];
	unsigned long revision;
	unsigned long version;
	unsigned long charmap_len;
	unsigned long charptr_len;
	unsigned long charmap_bpe;
	unsigned long charptr_bpe;
	unsigned char junk00[21];
	unsigned char family[64];
	unsigned char style[64];
	unsigned char junk01[1];
	unsigned short charmap_min;
	unsigned short charmap_max;
	unsigned char junk02[50];
	unsigned long fixedsize[2];
	unsigned char junk03[14];
	unsigned char table1_len;
	unsigned char table2_len;
	unsigned char table3_len;
	unsigned char advance_len;
	unsigned char junk04[102];
	unsigned long shadowmap_len;
	unsigned long shadowmap_bpe;
	unsigned char junk05[4];
	unsigned long shadowscale[2];
	//currently no need ;
} PGF_Header;

typedef struct intraFont {
	char* filename;
	unsigned char fileType;          
	unsigned char* fontdata;
	
	unsigned char* texture;          
	unsigned int texWidth;           
	unsigned int texHeight;          	
	unsigned short texX;
	unsigned short texY;
	unsigned short texYSize;
	
	unsigned short n_chars;
	char advancex;                   
	char advancey;                   
	unsigned char charmap_compr_len;
	unsigned short* charmap_compr;  
	unsigned short* charmap;         	
	Glyph* glyph;                    
	GlyphBW* glyphBW;
	
	unsigned short n_shadows;
	unsigned char shadowscale;       
	Glyph* shadowGlyph;              	
	
	float size;
	unsigned int color;
	unsigned int shadowColor;
	unsigned int options;

	struct intraFont* altFont;
} intraFont;
int intraFontInit(void);
void intraFontShutdown(void);
intraFont* intraFontLoad(const char *filename,unsigned int options);
void intraFontUnload(intraFont *font);
void intraFontActivate(intraFont *font);
void intraFontSetStyle(intraFont *font, float size, unsigned int color, unsigned int shadowColor, unsigned int options);
void intraFontSetEncoding(intraFont *font, unsigned int options);
void intraFontSetAltFont(intraFont *font, intraFont *altFont);
float intraFontPrintUCS2        (intraFont *font, float x, float y, const unsigned short *text);
float intraFontPrintUCS2Ex      (intraFont *font, float x, float y, const unsigned short *text, int length);
float intraFontPrintColumnUCS2  (intraFont *font, float x, float y, float width, const unsigned short *text);
float intraFontPrintColumnUCS2Ex(intraFont *font, float x, float y, float width, const unsigned short *text, int length);
float intraFontPrint        (intraFont *font, float x, float y, const char *text);
float intraFontPrintEx      (intraFont *font, float x, float y, const char *text, int length);
float intraFontPrintColumn  (intraFont *font, float x, float y, float width, const char *text);
float intraFontPrintColumnEx(intraFont *font, float x, float y, float width, const char *text, int length);
float intraFontPrintf        (intraFont *font, float x, float y, const char *text, ...);
float intraFontMeasureText  (intraFont *font, const char *text);
float intraFontMeasureTextEx(intraFont *font, const char *text, int length);
float intraFontMeasureTextUCS2  (intraFont *font, const unsigned short *text); 
float intraFontMeasureTextUCS2Ex(intraFont *font, const unsigned short *text, int length); 
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INTRAFONT_H__
