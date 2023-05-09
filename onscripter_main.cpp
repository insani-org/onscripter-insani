/* -*- C++ -*-
 * 
 *  onscripter_main.cpp -- main function of ONScripter
 *
 *  Copyright (c) 2001-2022 Ogapee. All rights reserved.
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
#include "version.h"

#if defined(INSANI)
#define INSANI_VERSION "202305xx \"FC01\""
#endif

ONScripter ons;

#if defined(IOS)
#import <Foundation/NSArray.h>
#import <UIKit/UIKit.h>
#import "DataCopier.h"
#import "DataDownloader.h"
#import "ScriptSelector.h"
#import "MoviePlayer.h"
#endif

#if defined(MACOSX) && defined(INSANI)
#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/NSString.h>
#import <Foundation/NSObject.h>
#import <Foundation/NSFileManager.h>
#endif

#if defined(PSP)
#include <pspkernel.h>
#include <psputility.h>
#include <psppower.h>
#include <ctype.h>

PSP_HEAP_SIZE_KB(-1);

int psp_power_resume_number = 0;

int exit_callback(int arg1, int arg2, void *common)
{
    ons.endCommand();
    sceKernelExitGame();
    return 0;
}

int power_callback(int unknown, int pwrflags, void *common)
{
    if (pwrflags & PSP_POWER_CB_RESUMING) psp_power_resume_number++;
    return 0;
}

int CallbackThread(SceSize args, void *argp)
{
    int cbid;
    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(0, cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int SetupCallbacks(void)
{
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, 0);
    return thid;
}
#endif

void optionHelp()
{
    printf( "Usage: onscripter [option ...]\n" );
    printf( "      --cdaudio\t\tuse CD audio if available\n");
    printf( "      --cdnumber no\tchoose the CD-ROM drive number\n");
    printf( "  -f, --font file\tset a TTF font file\n");
    printf( "      --registry file\tset a registry file\n");
    printf( "      --dll file\tset a dll file\n");
    printf( "  -r, --root path\tset the root path to the archives\n");
    printf( "      --fullscreen\tstart in fullscreen mode\n");
    printf( "      --window\t\tstart in windowed mode\n");
    printf( "      --force-button-shortcut\tignore useescspc and getenter command\n");
    printf( "      --enable-wheeldown-advance\tadvance the text on mouse wheel down\n");
    printf( "      --disable-rescale\tdo not rescale the images in the archives\n");
    printf( "      --render-font-outline\trender the outline of a text instead of casting a shadow\n");
    printf( "      --edit\t\tenable online modification of the volume and variables when 'z' is pressed\n");
    printf( "      --key-exe file\tset a file (*.EXE) that includes a key table\n");
    printf( "  -h, --help\t\tshow this help and exit\n");
    printf( "  -v, --version\t\tshow the version information and exit\n");
    exit(0);
}

void optionVersion()
{
    printf("Written by Ogapee <ogapee@aqua.dti2.ne.jp>\n\n");
    printf("Copyright (c) 2001-2022 Ogapee.\n");
    printf("This is free software; see the source for copying conditions.\n");
    exit(0);
}

#ifdef ANDROID
extern "C"
{
#include <jni.h>
#include <android/log.h>
#include <errno.h>
static JavaVM *jniVM = NULL;
static jobject JavaONScripter = NULL;
static jmethodID JavaPlayVideo = NULL;
static jmethodID JavaGetFD = NULL;
static jmethodID JavaMkdir = NULL;
static long *fd_start_offset = NULL;
static long *fd_length= NULL;
static long max_fd = -1;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jniVM = vm;
    return JNI_VERSION_1_2;
};

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    jniVM = vm;
};

#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_example_SanAngeles"
#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)

JNIEXPORT jint JNICALL JAVA_EXPORT_NAME(ONScripter_nativeInitJavaCallbacks) (JNIEnv * jniEnv, jobject thiz)
{
    JavaONScripter = jniEnv->NewGlobalRef(thiz);
    jclass JavaONScripterClass = jniEnv->GetObjectClass(JavaONScripter);
    JavaPlayVideo = jniEnv->GetMethodID(JavaONScripterClass, "playVideo", "([C)V");
    JavaGetFD = jniEnv->GetMethodID(JavaONScripterClass, "getFD", "([CI)[J");
    JavaMkdir = jniEnv->GetMethodID(JavaONScripterClass, "mkdir", "([C)I");
    return 0;
}

JNIEXPORT jint JNICALL 
JAVA_EXPORT_NAME(ONScripter_nativeGetWidth) ( JNIEnv*  env, jobject thiz )
{
	return ons.getWidth();
}

JNIEXPORT jint JNICALL 
JAVA_EXPORT_NAME(ONScripter_nativeGetHeight) ( JNIEnv*  env, jobject thiz )
{
	return ons.getHeight();
}

void playVideoAndroid(const char *filename)
{
    JNIEnv * jniEnv = NULL;
    jniVM->AttachCurrentThread(&jniEnv, NULL);

    if (!jniEnv){
        __android_log_print(ANDROID_LOG_ERROR, "ONS", "ONScripter::playVideoAndroid: Java VM AttachCurrentThread() failed");
        return;
    }

    jchar *jc = new jchar[strlen(filename)];
    for (int i=0 ; i<strlen(filename) ; i++)
        jc[i] = filename[i];
    jcharArray jca = jniEnv->NewCharArray(strlen(filename));
    jniEnv->SetCharArrayRegion(jca, 0, strlen(filename), jc);
    jniEnv->CallVoidMethod( JavaONScripter, JavaPlayVideo, jca );
    jniEnv->DeleteLocalRef(jca);
    delete[] jc;
}

static void resize_fd_buffer(int fd)
{
    if (max_fd < fd){
        long *tmp_fd_start_offset = fd_start_offset;
        fd_start_offset = new long[fd + 1];
        memset(fd_start_offset, sizeof(long)*(fd + 1), 0);
        
        long *tmp_fd_length = fd_length;
        fd_length = new long[fd + 1];
        memset(fd_length, sizeof(long)*(fd + 1), 0);
        
        if (max_fd >= 0){
            memcpy(fd_start_offset, tmp_fd_start_offset, sizeof(long)*(max_fd + 1));
            delete[] tmp_fd_start_offset;
            memcpy(fd_length, tmp_fd_length, sizeof(long)*(max_fd + 1));
            delete[] tmp_fd_length;
        }
        
        max_fd = fd;
    }
}

#undef fseek
int fseek_ons(FILE *stream, long offset, int whence)
{
    int fd = fileno(stream);
    
    if (whence == SEEK_SET)
        return fseek(stream, fd_start_offset[fd] + offset, whence);
    else if (whence == SEEK_CUR)
        return fseek(stream, offset, whence);
    
    return fseek(stream, fd_start_offset[fd] + fd_length[fd] + offset, SEEK_SET); // SEEK_END
}

#undef ftell
long ftell_ons(FILE *stream)
{
    int fd = fileno(stream);
    
    return ftell(stream) - fd_start_offset[fd];
}

#undef fgetc
int fgetc_ons(FILE *stream)
{
    int fd = fileno(stream);
    long pos = ftell(stream);
    long end_pos = fd_start_offset[fd] + fd_length[fd];
    
    if (pos >= end_pos) return EOF;
    
    return fgetc(stream);
}

#undef fgets
char *fgets_ons(char *s, int size, FILE *stream)
{
    int fd = fileno(stream);
    long pos = ftell(stream);
    long end_pos = fd_start_offset[fd] + fd_length[fd];
    
    if (pos + size >= end_pos){
        size = end_pos - pos;
        if (size <= 0) return NULL;
    }
    
    return fgets(s, size, stream);
}

#undef fread
size_t fread_ons(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int fd = fileno(stream);
    long pos = ftell(stream);
    long end_pos = fd_start_offset[fd] + fd_length[fd];
    
    if (pos + size*nmemb >= end_pos)
        nmemb = (end_pos - pos)/size;
    
    return fread(ptr, size, nmemb, stream);
}

#undef fopen
FILE *fopen_ons(const char *path, const char *mode)
{
    int mode2 = 0;
    if (mode[0] == 'w') mode2 = 1;

    FILE *fp = fopen(path, mode);
    if (fp){
        if (mode2 == 0){
            int fd = fileno(fp);
            resize_fd_buffer(fd);
            
            fd_start_offset[fd] = 0;
            fseek(fp, 0, SEEK_END);
            fd_length[fd] = ftell(fp);
            fseek(fp, 0, SEEK_SET);
        }
        
        return fp;
    }
    
    JNIEnv * jniEnv = NULL;
    jniVM->AttachCurrentThread(&jniEnv, NULL);

    if (!jniEnv){
        __android_log_print(ANDROID_LOG_ERROR, "ONS", "ONScripter::getFD: Java VM AttachCurrentThread() failed");
        return NULL;
    }

    jchar *jc = new jchar[strlen(path)];
    for (int i=0 ; i<strlen(path) ; i++)
        jc[i] = path[i];
    jcharArray jca = jniEnv->NewCharArray(strlen(path));
    jniEnv->SetCharArrayRegion(jca, 0, strlen(path), jc);
    jlongArray jla = (jlongArray)jniEnv->CallObjectMethod( JavaONScripter, JavaGetFD, jca, mode2 );
    jlong jl[3];
    jniEnv->GetLongArrayRegion(jla, 0, 3, jl);
    int fd = jl[0];
    jniEnv->DeleteLocalRef(jca);
    jniEnv->DeleteLocalRef(jla);
    delete[] jc;

    fp = fdopen(fd, mode);
    if (mode2 == 0 && fp){
        resize_fd_buffer(fd);
        
        if (jl[1] >= 0){
            fd_start_offset[fd] = jl[1];
            fd_length[fd] = jl[2];
        }
        else{
            fd_start_offset[fd] = 0;
            fseek(fp, 0, SEEK_END);
            fd_length[fd] = ftell(fp);
        }
        
        fseek(fp, fd_start_offset[fd], SEEK_SET);
    }
    
    return fp;
}

#undef mkdir
extern int mkdir(const char *pathname, mode_t mode);
int mkdir_ons(const char *pathname, mode_t mode)
{
    if (mkdir(pathname, mode) == 0 || errno != EACCES) return 0;
    
    JNIEnv * jniEnv = NULL;
    jniVM->AttachCurrentThread(&jniEnv, NULL);

    if (!jniEnv){
        __android_log_print(ANDROID_LOG_ERROR, "ONS", "ONScripter::mkdir: Java VM AttachCurrentThread() failed");
        return -1;
    }

    jchar *jc = new jchar[strlen(pathname)];
    for (int i=0 ; i<strlen(pathname) ; i++)
        jc[i] = pathname[i];
    jcharArray jca = jniEnv->NewCharArray(strlen(pathname));
    jniEnv->SetCharArrayRegion(jca, 0, strlen(pathname), jc);
    int ret = jniEnv->CallIntMethod( JavaONScripter, JavaMkdir, jca );
    jniEnv->DeleteLocalRef(jca);
    delete[] jc;

    return ret;
}
}
#endif

#if defined(IOS)
extern "C" void playVideoIOS(const char *filename, bool click_flag, bool loop_flag)
{
    NSString *str = [[NSString alloc] initWithUTF8String:filename];
    id obj = [MoviePlayer alloc];
    [[obj init] play:str click:click_flag loop:loop_flag];
    [obj release];
}
#endif

#if defined(QWS) || defined(ANDROID)
int SDL_main( int argc, char **argv )
#elif defined(PSP)
extern "C" int main( int argc, char **argv )
#else
int main( int argc, char **argv )
#endif
{
    printf("ONScripter version %s(%d.%02d)\n", ONS_VERSION, NSC_VERSION/100, NSC_VERSION%100 );

#if defined(INSANI)
    int uwu = rand() % 3;
    switch(uwu)
    {
        case 2:
            printf("HEWWO UWU %s UWUS YOUWU\n", INSANI_VERSION);
            break;
        case 1:
            printf("ERO\\4TW Unprotected Mode Run-time version %s\n", INSANI_VERSION);
            printf("Copyright (c) Irrational Insanity Spirit, Inc. 2005-2023\n");
            break;
        case 0:
        default:
            printf("Autodetect: OYABB neo-insanity spirit (%s)\n", INSANI_VERSION);
            break;
    }
#endif

#if defined(PSP)
    ons.disableRescale();
    ons.enableButtonShortCut();
    SetupCallbacks();
#elif defined(WINCE)
    char currentDir[256];
    strcpy(currentDir, argv[0]);
    char* cptr = currentDir;
    int i, len = strlen(currentDir);
    for(i=len-1; i>0; i--){
        if(cptr[i] == '\\' || cptr[i] == '/')
            break;
    }
    cptr[i] = '\0';
    ons.setArchivePath(currentDir);
    ons.disableRescale();
    ons.enableButtonShortCut();
#elif defined(ANDROID) 
    ons.enableButtonShortCut();
#endif

#if defined(IOS)
#if defined(HAVE_CONTENTS)
    if ([[[DataCopier alloc] init] copy]) exit(-1);
#endif

    // scripts and archives are stored under /Library/Caches
    NSArray* cpaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cpath = [[cpaths objectAtIndex:0] stringByAppendingPathComponent:@"ONS"];
    char filename[256];
    strcpy(filename, [cpath UTF8String]);
    ons.setArchivePath(filename);

    // output files are stored under /Documents
    NSArray* dpaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* dpath = [[dpaths objectAtIndex:0] stringByAppendingPathComponent:@"ONS"];
    strcpy(filename, [dpath UTF8String]);
    ons.setSaveDir(filename);

#if defined(ZIP_URL)
    if ([[[DataDownloader alloc] init] download]) exit(-1);
#endif

#if defined(USE_SELECTOR)
    // scripts and archives are stored under /Library/Caches
    cpath = [[[ScriptSelector alloc] initWithStyle:UITableViewStylePlain] select];
    strcpy(filename, [cpath UTF8String]);
    ons.setArchivePath(filename);

    // output files are stored under /Documents
    dpath = [[dpaths objectAtIndex:0] stringByAppendingPathComponent:[cpath lastPathComponent]];
    NSFileManager *fm = [NSFileManager defaultManager];
    [fm createDirectoryAtPath:dpath withIntermediateDirectories: YES attributes: nil error:nil];
    strcpy(filename, [dpath UTF8String]);
    ons.setSaveDir(filename);
#endif

#if defined(RENDER_FONT_OUTLINE)
    ons.renderFontOutline();
#endif
#endif

#if defined(MACOSX) && defined(INSANI) && defined(APPBUNDLE)
    // scripts and archives are stored under AppName.app/Contents/Resources
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char resourcesPath[255];
    if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)resourcesPath, 255)) ons.setArchivePath(resourcesPath);

    // store output files under ~/Documents/ONScripter/AppName
    NSString *appName = [NSString stringWithUTF8String:getprogname()];
    NSFileManager *fm = [NSFileManager defaultManager];
    NSString *rootPath = [@"~/Documents/ONScripter" stringByExpandingTildeInPath];
    NSString *savePath = [rootPath stringByAppendingPathComponent:appName];
    char savefilesPath[255];
    strcpy(savefilesPath, [savePath UTF8String]);
    if ([fm fileExistsAtPath:savePath isDirectory:NULL]) ons.setSaveDir(savefilesPath);
    else
    {
        [fm createDirectoryAtPath:savePath withIntermediateDirectories: YES attributes: nil error: nil];
        ons.setSaveDir(savefilesPath);
    }
#endif

    // ----------------------------------------
    // Parse options
    argv++;
    while( argc > 1 ){
        if ( argv[0][0] == '-' ){
            if ( !strcmp( argv[0]+1, "h" ) || !strcmp( argv[0]+1, "-help" ) ){
                optionHelp();
            }
            else if ( !strcmp( argv[0]+1, "v" ) || !strcmp( argv[0]+1, "-version" ) ){
                optionVersion();
            }
            else if ( !strcmp( argv[0]+1, "-cdaudio" ) ){
                ons.enableCDAudio();
            }
            else if ( !strcmp( argv[0]+1, "-cdnumber" ) ){
                argc--;
                argv++;
                ons.setCDNumber(atoi(argv[0]));
            }
            else if ( !strcmp( argv[0]+1, "f" ) || !strcmp( argv[0]+1, "-font" ) ){
                argc--;
                argv++;
                ons.setFontFile(argv[0]);
            }
            else if ( !strcmp( argv[0]+1, "-registry" ) ){
                argc--;
                argv++;
                ons.setRegistryFile(argv[0]);
            }
            else if ( !strcmp( argv[0]+1, "-dll" ) ){
                argc--;
                argv++;
                ons.setDLLFile(argv[0]);
            }
            else if ( !strcmp( argv[0]+1, "r" ) || !strcmp( argv[0]+1, "-root" ) ){
                argc--;
                argv++;
                ons.setArchivePath(argv[0]);
            }
            else if ( !strcmp( argv[0]+1, "-fullscreen" ) ){
                ons.setFullscreenMode();
            }
            else if ( !strcmp( argv[0]+1, "-window" ) ){
                ons.setWindowMode();
            }
            else if ( !strcmp( argv[0]+1, "-force-button-shortcut" ) ){
                ons.enableButtonShortCut();
            }
            else if ( !strcmp( argv[0]+1, "-enable-wheeldown-advance" ) ){
                ons.enableWheelDownAdvance();
            }
            else if ( !strcmp( argv[0]+1, "-disable-rescale" ) ){
                ons.disableRescale();
            }
            else if ( !strcmp( argv[0]+1, "-render-font-outline" ) ){
                ons.renderFontOutline();
            }
            else if ( !strcmp( argv[0]+1, "-edit" ) ){
                ons.enableEdit();
            }
            else if ( !strcmp( argv[0]+1, "-key-exe" ) ){
                argc--;
                argv++;
                ons.setKeyEXE(argv[0]);
            }
#if defined(ANDROID) 
            else if ( !strcmp( argv[0]+1, "-open-only" ) ){
                argc--;
                argv++;
                if (ons.openScript()) exit(-1);
                return 0;
            }
#endif
            else{
                printf(" unknown option %s\n", argv[0] );
            }
        }
        else{
            optionHelp();
        }
        argc--;
        argv++;
    }
    
    // ----------------------------------------
    // Run ONScripter

#if !defined(ANDROID) 
    if (ons.openScript()) exit(-1);
#endif
    if (ons.init()) exit(-1);
    ons.executeLabel();
    
    exit(0);
}
