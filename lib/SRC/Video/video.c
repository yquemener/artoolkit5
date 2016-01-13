/*
 *  video.c
 *  ARToolKit5
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015 Daqri, LLC.
 *  Copyright 2003-2015 ARToolworks, Inc.
 *
 *  Author(s): Hirokazu Kato, Philip Lamb
 *
 */
/*
 *   author: Hirokazu Kato ( kato@sys.im.hiroshima-cu.ac.jp )
 *
 *   Revision: 6.0   Date: 2003/09/29
 */
#include <stdio.h>
#include <AR/video.h>

static AR2VideoParamT* gPlatformGenericVidParam = NULL;

int arVideoGetDefaultDevice( void )
{
#if defined(AR_DEFAULT_INPUT_V4L)
    return AR_VIDEO_DEVICE_V4L;
#elif defined(AR_DEFAULT_INPUT_V4L2)
    return AR_VIDEO_DEVICE_V4L2;
#elif defined(AR_DEFAULT_INPUT_DV)
    return AR_VIDEO_DEVICE_DV;
#elif defined(AR_DEFAULT_INPUT_1394CAM)
    return AR_VIDEO_DEVICE_1394CAM;
#elif defined(AR_DEFAULT_INPUT_SGI)
    return AR_VIDEO_DEVICE_SGI;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_DIRECTSHOW)
    return AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_DSVIDEOLIB)
    return AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_DRAGONFLY)
    return AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY;
#elif defined(AR_DEFAULT_INPUT_QUICKTIME)
    return AR_VIDEO_DEVICE_QUICKTIME;
#elif defined(AR_DEFAULT_INPUT_GSTREAMER)
    return AR_VIDEO_DEVICE_GSTREAMER;
#elif defined(AR_DEFAULT_INPUT_IPHONE)
    return AR_VIDEO_DEVICE_IPHONE;
#elif defined(AR_DEFAULT_INPUT_QUICKTIME7)
    return AR_VIDEO_DEVICE_QUICKTIME7;
#elif defined(AR_DEFAULT_INPUT_IMAGE)
    return AR_VIDEO_DEVICE_IMAGE;
#elif defined(AR_DEFAULT_INPUT_ANDROID)
    return AR_VIDEO_DEVICE_ANDROID;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_MEDIA_FOUNDATION)
    return AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_MEDIA_CAPTURE)
    return AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE;
#else
    return AR_VIDEO_DEVICE_DUMMY;
#endif
}

int arVideoOpen( const char *config )
{
    if( gPlatformGenericVidParam != NULL ) {
        ARLOGe("arVideoOpen: Error, video device already open.\n");
        return -1;
    }
    gPlatformGenericVidParam = ar2VideoOpen( config );
    if( gPlatformGenericVidParam == NULL ) return -1;

    return 0;
}

int arVideoOpenAsync(const char *config, void (*callback)(void *), void *userdata)
{
    if( gPlatformGenericVidParam != NULL ) {
        ARLOGe("arVideoOpenAsync: Error, video device already open.\n");
        return -1;
    }
    gPlatformGenericVidParam = ar2VideoOpenAsync(config, callback, userdata);
    if( gPlatformGenericVidParam == NULL ) return -1;

    return 0;
}

int arVideoClose( void )
{
    int     ret;

    if( gPlatformGenericVidParam == NULL ) return -1;

    ret = ar2VideoClose( gPlatformGenericVidParam );
    gPlatformGenericVidParam = NULL;

    return ret;
}

int arVideoDispOption( void )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return  ar2VideoDispOption( gPlatformGenericVidParam );
}

int arVideoGetDevice( void )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetDevice(gPlatformGenericVidParam);
}

int arVideoGetId( ARUint32 *id0, ARUint32 *id1 )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetId( gPlatformGenericVidParam, id0, id1 );
}

int arVideoGetSize( int *x, int *y )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetSize( gPlatformGenericVidParam, x, y );
}

int arVideoGetPixelSize( void )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetPixelSize( gPlatformGenericVidParam );
}

AR_PIXEL_FORMAT arVideoGetPixelFormat( void )
{
    if( gPlatformGenericVidParam == NULL ) return ((AR_PIXEL_FORMAT)-1);

    return ar2VideoGetPixelFormat( gPlatformGenericVidParam );
}

ARUint8 *arVideoGetImage( void )
{
    AR2VideoBufferT *buffer;

    if( gPlatformGenericVidParam == NULL ) return NULL;

    buffer = ar2VideoGetImage(gPlatformGenericVidParam);
	if (buffer == NULL) return (NULL);
    if( buffer->fillFlag == 0 ) return NULL;

    return buffer->buff;
}

int arVideoCapStart( void )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoCapStart( gPlatformGenericVidParam );
}

int arVideoCapStartAsync(AR_VIDEO_FRAME_READY_CALLBACK callback, void *userdata)
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoCapStartAsync(gPlatformGenericVidParam, callback, userdata);
}

int arVideoCapStop( void )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoCapStop( gPlatformGenericVidParam );
}

int   arVideoGetParami( int paramName, int *value )
{
    if (paramName == AR_VIDEO_GET_VERSION) return (ar2VideoGetParami(NULL, AR_VIDEO_GET_VERSION, NULL));

    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetParami( gPlatformGenericVidParam, paramName, value );
}

int   arVideoSetParami( int paramName, int  value )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoSetParami( gPlatformGenericVidParam, paramName, value );
}

int   arVideoGetParamd( int paramName, double *value )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetParamd( gPlatformGenericVidParam, paramName, value );
}

int   arVideoSetParamd( int paramName, double  value )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoSetParamd( gPlatformGenericVidParam, paramName, value );
}

int   arVideoSaveParam( char *filename )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoSaveParam( gPlatformGenericVidParam, filename );
}

int   arVideoLoadParam( char *filename )
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoLoadParam( gPlatformGenericVidParam, filename );
}

int arVideoSetBufferSize(const int width, const int height)
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoSetBufferSize( gPlatformGenericVidParam, width, height );
}

int arVideoGetBufferSize(int *width, int *height)
{
    if( gPlatformGenericVidParam == NULL ) return -1;

    return ar2VideoGetBufferSize( gPlatformGenericVidParam, width, height );
}

int arVideoGetCParam(ARParam *cparam)
{
    if (gPlatformGenericVidParam == NULL) return -1;

    return ar2VideoGetCParam(gPlatformGenericVidParam, cparam);
}

int arVideoGetCParamAsync(void (*callback)(const ARParam *, void *), void *userdata)
{
    if (gPlatformGenericVidParam == NULL) return -1;

    return ar2VideoGetCParamAsync(gPlatformGenericVidParam, callback, userdata);;
}

// N.B. This function is duplicated in libAR, so that libAR doesn't need to
// link to libARvideo. Therefore, if changes are made here they should be duplicated there.
int arVideoUtilGetPixelSize( const AR_PIXEL_FORMAT arPixelFormat )
{
    switch( arPixelFormat ) {
        case AR_PIXEL_FORMAT_RGB:
        case AR_PIXEL_FORMAT_BGR:
            return 3;
        case AR_PIXEL_FORMAT_RGBA:
        case AR_PIXEL_FORMAT_BGRA:
        case AR_PIXEL_FORMAT_ABGR:
        case AR_PIXEL_FORMAT_ARGB:
            return 4;
        case AR_PIXEL_FORMAT_MONO:
        case AR_PIXEL_FORMAT_420v: // Report only size of luma pixels (i.e. plane 0).
        case AR_PIXEL_FORMAT_420f: // Report only size of luma pixels (i.e. plane 0).
        case AR_PIXEL_FORMAT_NV21: // Report only size of luma pixels (i.e. plane 0).
            return 1;
        case AR_PIXEL_FORMAT_2vuy:
        case AR_PIXEL_FORMAT_yuvs:
        case AR_PIXEL_FORMAT_RGB_565:
        case AR_PIXEL_FORMAT_RGBA_5551:
        case AR_PIXEL_FORMAT_RGBA_4444:
            return 2;
        default:
            return (0);
    }
}

// N.B. This function is duplicated in libAR, so that libAR doesn't need to
// link to libARvideo. Therefore, if changes are made here they should be duplicated there.
const char *arVideoUtilGetPixelFormatName(const AR_PIXEL_FORMAT arPixelFormat)
{
    const char *names[] = {
        "AR_PIXEL_FORMAT_RGB",
        "AR_PIXEL_FORMAT_BGR",
        "AR_PIXEL_FORMAT_RGBA",
        "AR_PIXEL_FORMAT_BGRA",
        "AR_PIXEL_FORMAT_ABGR",
        "AR_PIXEL_FORMAT_MONO",
        "AR_PIXEL_FORMAT_ARGB",
        "AR_PIXEL_FORMAT_2vuy",
        "AR_PIXEL_FORMAT_yuvs",
        "AR_PIXEL_FORMAT_RGB_565",
        "AR_PIXEL_FORMAT_RGBA_5551",
        "AR_PIXEL_FORMAT_RGBA_4444",
        "AR_PIXEL_FORMAT_420v",
        "AR_PIXEL_FORMAT_420f",
        "AR_PIXEL_FORMAT_NV21"
    };
    if ((int)arPixelFormat < 0 || (int)arPixelFormat > AR_PIXEL_FORMAT_MAX) {
        ARLOGe("arVideoUtilGetPixelFormatName: Error, unrecognised pixel format (%d).\n", (int)arPixelFormat);
        return (NULL);
    }
    return (names[(int)arPixelFormat]);
}
