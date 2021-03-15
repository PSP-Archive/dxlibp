TARGET_LIB = dxlibp_UTF8.a

#general functions
OBJS += general/general.o
OBJS += general/dxlib_init.o
OBJS += general/dxlib_isinit.o
OBJS += general/dxlib_end.o
OBJS += general/processmessage.o
OBJS += general/getnowcount.o
OBJS += general/getnowhiperformancecount.o
OBJS += general/getdatetime.o
OBJS += general/srand.o
OBJS += general/getrand.o
OBJS += general/setcodepoint.o
OBJS += general/changerandmode.o
OBJS += general/changecpuspeed.o
OBJS += general/sleep.o

#threadsafe alloc/free
OBJS += safealloc/safealloc.o

#file io functions
OBJS += fileio/fileio.o
OBJS += fileio/fileread_open.o
OBJS += fileio/fileread_close.o
OBJS += fileio/fileread_eof.o
OBJS += fileio/fileread_idlecheck.o
OBJS += fileio/fileread_read.o
OBJS += fileio/fileread_seek.o
OBJS += fileio/fileread_tell.o
OBJS += fileio/fileread_size.o
OBJS += fileio/fileread_getc.o
OBJS += fileio/fileread_gets.o

#input functions
OBJS += input/input.o
OBJS += input/getinputstate.o
OBJS += input/getjoypadnum.o
OBJS += input/getjoypadinputstate.o
OBJS += input/getjoypadanaloginput.o
OBJS += input/getjoypadanaloginputright.o
OBJS += input/setjoypadinputtokeyinput.o
OBJS += input/setkeyinputtopspinput.o
OBJS += input/startjoypadvibration.o
OBJS += input/stopjoypadvibration.o
OBJS += input/checkhitkeyall.o
OBJS += input/checkhitkey.o
OBJS += input/gethitkeystateall.o

#math functions
#dxlibp.hに内容をinline化して移してあります。コード保守のために遺してあるソースなので消さないでください。
#OBJS += math/vector.c
#OBJS += math/matrix.c

#graphics functions
OBJS += graphics/drawline.o
OBJS += graphics/drawbox.o
OBJS += graphics/drawcircle.o
OBJS += graphics/drawoval.o
OBJS += graphics/drawtriangle.o
OBJS += graphics/drawpixel.o
OBJS += graphics/getpixel.o
OBJS += graphics/makegraph.o
OBJS += graphics/getgraphsize.o
OBJS += graphics/graphics.o
OBJS += graphics/texutils.o
OBJS += graphics/loadgraphscreen.o
OBJS += graphics/loadgraph.o
OBJS += graphics/loaddivgraph.o
OBJS += graphics/derivationgraph.o
OBJS += graphics/drawgraph.o
OBJS += graphics/drawturngraph.o
OBJS += graphics/drawextendgraph.o
OBJS += graphics/drawmodigraph.o
OBJS += graphics/drawmodigraphf.o
OBJS += graphics/drawrotagraph.o
OBJS += graphics/drawrotagraph2.o
OBJS += graphics/drawrotagraphf.o
OBJS += graphics/drawrotagraph2f.o
OBJS += graphics/drawrectgraph.o
OBJS += graphics/getdrawscreengraph.o
OBJS += graphics/getscreenstate.o
OBJS += graphics/screenflip.o
OBJS += graphics/screencopy.o
OBJS += graphics/setdrawarea.o
OBJS += graphics/setgraphmode.o
OBJS += graphics/setdrawmode.o
OBJS += graphics/setdrawbright.o
OBJS += graphics/setdrawblendmode.o
OBJS += graphics/settranscolor.o
OBJS += graphics/cleardrawscreen.o
OBJS += graphics/setdrawscreen.o
OBJS += graphics/waitgpusync.o
OBJS += graphics/setwaitvsyncflag.o
OBJS += graphics/convertgraphformat.o
OBJS += graphics/getdisplayformat.o
OBJS += graphics/setdisplayformat.o

OBJS += graphics/dxppng.o

#intrafont
OBJS += font/intraFont.o
OBJS += font/font.o
OBJS += font/createfonttohandle.o
OBJS += font/deletefonttohandle.o
OBJS += font/getdrawstringwidthtohandle.o

#debug functions
OBJS += debug/debug.o
OBJS += debug/clsdx.o
OBJS += debug/printfdx.o
OBJS += debug/applogadd.o

OBJS += debug/dxpdebugputs_zenkaku.o

#cpcnv functions
OBJS += cpcnv/liblzr.o
OBJS += cpcnv/cpcnv.o
OBJS += cpcnv/sjis/multibytecharcheck.o
OBJS += cpcnv/sjis/sjisstrlen.o
OBJS += cpcnv/sjis/sjisfromucs2.o
OBJS += cpcnv/sjis/sjistoucs2.o
OBJS += cpcnv/utf8/utf8toucs2.o
OBJS += cpcnv/utf8/utf8fromucs2.o
OBJS += cpcnv/utf8/utf8strlen.o
OBJS += cpcnv/ucs2/ucs2strlen.o

#on screen keyboard functions
OBJS += osk/osk.o

#sound functions
OBJS += sound/sound.o
OBJS += sound/soundcodec.o
OBJS += sound/soundthread.o
OBJS += sound/mp3.o
#OBJS += sound/initsoundmem.o
#OBJS += sound/loadsoundmem.o
#OBJS += sound/playsoundmem.o
#OBJS += sound/checksoundmem.o
#OBJS += sound/stopsoundmem.o
#OBJS += sound/deletesoundmem.o
#OBJS += sound/setlooppossoundmem.o
#OBJS += sound/setloopsamplepossoundmem.o
#OBJS += sound/changevolumesoundmem.o
#OBJS += sound/setpansoundmem.o
#OBJS += sound/setcreatesounddatatype.o

#3d graphics functions
OBJS += graphics/3d/3dgraphics.o
OBJS += graphics/3d/drawline3d.o
OBJS += graphics/3d/drawpolygon3d.o
OBJS += graphics/3d/drawtriangle3d.o
OBJS += graphics/3d/camera.o

INCDIR = 
CFLAGS = -G0 -Wall -O3# -save-temps # -pg
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

doc :
	doxygen