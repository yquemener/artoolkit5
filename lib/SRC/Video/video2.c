/*
 *  video2.c
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
 *  Copyright 2002-2015 ARToolworks, Inc.
 *
 *  Author(s): Hirokazu Kato ( kato@sys.im.hiroshima-cu.ac.jp ),
 *             Philip Lamb
 *
 *  Updates: John Wolf
 *
 *  Revision: 6.0   Date: 2003/09/29
 */

#include <stdio.h>
#include <string.h>
#include <AR/video.h>
#include <AR/config.h>

static const char *ar2VideoGetConfig(const char *config_in)
{
    const char *config = NULL;

    /* If no config string is supplied, we should use the environment variable, otherwise set a sane default */
    if (!config_in || !(config_in[0])) {
        /* None supplied, lets see if the user supplied one from the shell */
#ifndef _WINRT
        char *envconf = getenv("ARTOOLKIT5_VCONF");
        if (envconf && envconf[0]) {
            config = envconf;
            ARLOGi("Using video config from environment \"%s\".\n", envconf);
        } else {
#endif // !_WINRT
            config = NULL;
            ARLOGi("Using default video config.\n");
#ifndef _WINRT
        }
#endif // !_WINRT
    } else {
        config = config_in;
        ARLOGi("Using supplied video config \"%s\".\n", config_in);
    }

    return config;
}

static int ar2VideoGetDeviceWithConfig(const char *config, const char **configStringFollowingDevice_p)
{
    int                        device;
    const char                *a;
    char                       b[256];

    device = arVideoGetDefaultDevice();

    if (configStringFollowingDevice_p) *configStringFollowingDevice_p = NULL;

    //TODO: John Wolf - make this argument processing logic case insensitive.
    a = config;
    if (a) {
        for(;;) {
            while( *a == ' ' || *a == '\t' ) a++;
            if( *a == '\0' ) break;

            if( sscanf(a, "%s", b) == 0 ) break;

            if( strcmp( b, "-device=Dummy" ) == 0 )             {
                device = AR_VIDEO_DEVICE_DUMMY;
            }
            else if( strcmp( b, "-device=LinuxV4L" ) == 0 )     {
                device = AR_VIDEO_DEVICE_V4L;
            }
            else if( strcmp( b, "-device=LinuxV4L2" ) == 0 )     {
                device = AR_VIDEO_DEVICE_V4L2;
            }
            else if( strcmp( b, "-device=LinuxDV" ) == 0 )      {
                device = AR_VIDEO_DEVICE_DV;
            }
            else if( strcmp( b, "-device=Linux1394Cam" ) == 0 ) {
                device = AR_VIDEO_DEVICE_1394CAM;
            }
            else if( strcmp( b, "-device=SGI" ) == 0 )          {
                device = AR_VIDEO_DEVICE_SGI;
            }
            else if( strcmp( b, "-device=WinDS" ) == 0 )      {
                device = AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW;
            }
            else if( strcmp( b, "-device=WinDF" ) == 0 )      {
                device = AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY;
            }
            else if( strcmp( b, "-device=WinDSVL" ) == 0 )      {
                device = AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB;
                if (configStringFollowingDevice_p) *configStringFollowingDevice_p = a;
            }
            else if( strcmp( b, "-device=QUICKTIME" ) == 0 )    {
                device = AR_VIDEO_DEVICE_QUICKTIME;
            }
            else if( strcmp( b, "-device=GStreamer" ) == 0 )    {
                device = AR_VIDEO_DEVICE_GSTREAMER;
                if (configStringFollowingDevice_p) *configStringFollowingDevice_p = a;
            }
            else if( strcmp( b, "-device=iPhone" ) == 0 )    {
                device = AR_VIDEO_DEVICE_IPHONE;
            }
            else if( strcmp( b, "-device=QuickTime7" ) == 0 )    {
                device = AR_VIDEO_DEVICE_QUICKTIME7;
            }
            else if( strcmp( b, "-device=Image" ) == 0 )    {
                device = AR_VIDEO_DEVICE_IMAGE;
            }
            else if( strcmp( b, "-device=Android" ) == 0 )    {
                device = AR_VIDEO_DEVICE_ANDROID;
            }
            else if( strcmp( b, "-device=WinMF" ) == 0 )    {
                device = AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION;
            }
            else if( strcmp( b, "-device=WinMC" ) == 0 )    {
                device = AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE;
            }

            while( *a != ' ' && *a != '\t' && *a != '\0') a++;
        }
    }

    if (configStringFollowingDevice_p) {
        if (*configStringFollowingDevice_p) {
            while( **configStringFollowingDevice_p != ' ' && **configStringFollowingDevice_p != '\t' && **configStringFollowingDevice_p != '\0') (*configStringFollowingDevice_p)++;
            while( **configStringFollowingDevice_p == ' ' || **configStringFollowingDevice_p == '\t') (*configStringFollowingDevice_p)++;
        } else {
            *configStringFollowingDevice_p = config;
        }
    }

    return (device);
}

ARVideoSourceInfoListT *ar2VideoCreateSourceInfoList(const char *config_in)
{
    int device = ar2VideoGetDeviceWithConfig(ar2VideoGetConfig(config_in), NULL);
#ifdef AR_INPUT_DUMMY
    if (device == AR_VIDEO_DEVICE_DUMMY) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_V4L
    if (device == AR_VIDEO_DEVICE_V4L) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_V4L2
    if (device == AR_VIDEO_DEVICE_V4L2) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_DV
    if (device == AR_VIDEO_DEVICE_DV) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_1394CAM
    if (device == AR_VIDEO_DEVICE_1394CAM) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if (device == AR_VIDEO_DEVICE_GSTREAMER) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_SGI
    if (device == AR_VIDEO_DEVICE_SGI) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if (device == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW) {
		return ar2VideoCreateSourceInfoListWinDS(config_in);
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if (device == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if (device == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if (device == AR_VIDEO_DEVICE_QUICKTIME) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_IPHONE
    if (device == AR_VIDEO_DEVICE_IPHONE) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if (device == AR_VIDEO_DEVICE_QUICKTIME7) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_IMAGE
    if (device == AR_VIDEO_DEVICE_IMAGE) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_ANDROID
    if (device == AR_VIDEO_DEVICE_ANDROID) {
        return (NULL);
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if (device == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION) {
        return ar2VideoCreateSourceInfoListWinMF(config_in);
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if (device == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE) {
        return (NULL);
    }
#endif
    return (NULL);
}

void ar2VideoDeleteSourceInfoList(ARVideoSourceInfoListT **p)
{
    int i;

    if (!p || !*p) return;

    for (i = 0; i < (*p)->count; i++) {
        free((*p)->info[i].name);
        free((*p)->info[i].UID);
    }
    free((*p)->info);
    free(*p);

    *p = NULL;
}

AR2VideoParamT *ar2VideoOpen( const char *config_in )
{
    AR2VideoParamT *platformGenericVidParam;
    const char     *config;
    // Some devices won't understand the "-device=" option, so we need to pass
    // only the portion following that option to them.
    const char     *configStringFollowingDevice = NULL;

    arMalloc( platformGenericVidParam, AR2VideoParamT, 1 );
    config = ar2VideoGetConfig(config_in);
    platformGenericVidParam->deviceType = ar2VideoGetDeviceWithConfig(config, &configStringFollowingDevice);

    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
#ifdef AR_INPUT_DUMMY
        if( (platformGenericVidParam->device.dummy = ar2VideoOpenDummy(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"Dummy\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
#ifdef AR_INPUT_V4L
        if( (platformGenericVidParam->device.v4l = ar2VideoOpenV4L(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"LinuxV4L\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
#ifdef AR_INPUT_V4L2
        if( (platformGenericVidParam->device.v4l2 = ar2VideoOpenV4L2(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"LinuxV4L2\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
#ifdef AR_INPUT_DV
        if( (platformGenericVidParam->device.dv = ar2VideoOpenDv(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"LinuxDV\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
#ifdef AR_INPUT_1394CAM
        if( (platformGenericVidParam->device.cam1394 = ar2VideoOpen1394(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"Linux1394Cam\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
#ifdef AR_INPUT_GSTREAMER
        if( (platformGenericVidParam->device.gstreamer = ar2VideoOpenGStreamer(configStringFollowingDevice)) != NULL )
            return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"GStreamer\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
#ifdef AR_INPUT_SGI
        if( (platformGenericVidParam->device.sgi = ar2VideoOpenSGI(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"SGI\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
        if( (platformGenericVidParam->device.winDS = ar2VideoOpenWinDS(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"WinDS\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
        if( (platformGenericVidParam->device.winDSVL = ar2VideoOpenWinDSVL(configStringFollowingDevice)) != NULL )
            return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"WinDSVL\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
        if( (platformGenericVidParam->device.winDF = ar2VideoOpenWinDF(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"WinDF\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
#ifdef AR_INPUT_QUICKTIME
        if( (platformGenericVidParam->device.quickTime = ar2VideoOpenQuickTime(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"QUICKTIME\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
#ifdef AR_INPUT_IPHONE
        if ((platformGenericVidParam->device.iPhone = ar2VideoOpeniPhone(config)) != NULL) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"iPhone\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
#ifdef AR_INPUT_QUICKTIME7
        if( (platformGenericVidParam->device.quickTime7 = ar2VideoOpenQuickTime7(config)) != NULL )
            return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"QuickTime7\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
#ifdef AR_INPUT_IMAGE
        if( (platformGenericVidParam->device.image = ar2VideoOpenImage(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"Image\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
#ifdef AR_INPUT_ANDROID
        if( (platformGenericVidParam->device.android = ar2VideoOpenAndroid(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"Android\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
        if( (platformGenericVidParam->device.winMF = ar2VideoOpenWinMF(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"WinMF\" not supported on this build/architecture/system.\n");
#endif
    }
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
        if( (platformGenericVidParam->device.winMC = ar2VideoOpenWinMC(config)) != NULL ) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"WinMC\" not supported on this build/architecture/system.\n");
#endif
    }

    free( platformGenericVidParam );
    return (AR2VideoParamT*)NULL;
}

AR2VideoParamT *ar2VideoOpenAsync(const char *config_in, void (*callback)(void *), void *userdata)
{
    AR2VideoParamT            *platformGenericVidParam;
    const char                *config;
    // Some devices won't understand the "-device=" option, so we need to pass
    // only the portion following that option to them.
    const char                *configStringFollowingDevice = NULL;

    arMalloc( platformGenericVidParam, AR2VideoParamT, 1 );
    config = ar2VideoGetConfig(config_in);
    platformGenericVidParam->deviceType = ar2VideoGetDeviceWithConfig(config, &configStringFollowingDevice);

    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
#ifdef AR_INPUT_IPHONE
        if (callback) platformGenericVidParam->device.iPhone = ar2VideoOpenAsynciPhone(config, callback, userdata);
        else platformGenericVidParam->device.iPhone = NULL;

        if (platformGenericVidParam->device.iPhone != NULL) return platformGenericVidParam;
#else
        ARLOGe("ar2VideoOpen: Error: device \"iPhone\" not supported on this build/architecture/system.\n");
#endif
    }

    free( platformGenericVidParam );
    return(AR2VideoParamT*)NULL;
}

int ar2VideoClose( AR2VideoParamT *platformGenericVidParam )
{
    int ret;

    if (!platformGenericVidParam) return -1;
    ret = -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        ret = ar2VideoCloseDummy( platformGenericVidParam->device.dummy );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        ret = ar2VideoCloseV4L( platformGenericVidParam->device.v4l );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        ret = ar2VideoCloseV4L2( platformGenericVidParam->device.v4l2 );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        ret = ar2VideoCloseDv( platformGenericVidParam->device.dv );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        ret = ar2VideoClose1394( platformGenericVidParam->device.cam1394 );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        ret = ar2VideoCloseGStreamer( platformGenericVidParam->device.gstreamer );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        ret = ar2VideoCloseSGI( platformGenericVidParam->device.sgi );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        ret = ar2VideoCloseWinDS( platformGenericVidParam->device.winDS );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        ret = ar2VideoCloseWinDSVL( platformGenericVidParam->device.winDSVL );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        ret = ar2VideoCloseWinDF( platformGenericVidParam->device.winDF );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        ret = ar2VideoCloseQuickTime( platformGenericVidParam->device.quickTime );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        ret = ar2VideoCloseiPhone( platformGenericVidParam->device.iPhone );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        ret = ar2VideoCloseQuickTime7( platformGenericVidParam->device.quickTime7 );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        ret = ar2VideoCloseImage( platformGenericVidParam->device.image );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        ret = ar2VideoCloseAndroid( platformGenericVidParam->device.android );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        ret = ar2VideoCloseWinMF( platformGenericVidParam->device.winMF );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        ret = ar2VideoCloseWinMC( platformGenericVidParam->device.winMC );
    }
#endif
    free (platformGenericVidParam);
    return (ret);
}

int ar2VideoDispOption( AR2VideoParamT *platformGenericVidParam )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoDispOptionDummy();
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoDispOptionV4L();
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoDispOptionV4L2();
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoDispOptionDv();
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoDispOption1394();
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoDispOptionGStreamer();
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoDispOptionSGI();
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoDispOptionWinDS();
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoDispOptionWinDSVL();
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoDispOptionWinDF();
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoDispOptionQuickTime();
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoDispOptioniPhone();
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoDispOptionQuickTime7();
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoDispOptionImage();
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoDispOptionAndroid();
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoDispOptionWinMF();
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoDispOptionWinMC();
    }
#endif
    return (-1);
}

int ar2VideoGetDevice( AR2VideoParamT *platformGenericVidParam )
{
    if (!platformGenericVidParam) return -1;
    return platformGenericVidParam->deviceType;
}

int ar2VideoGetId( AR2VideoParamT *platformGenericVidParam, ARUint32 *id0, ARUint32 *id1 )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetIdDummy( platformGenericVidParam->device.dummy, id0, id1 );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoGetIdV4L( platformGenericVidParam->device.v4l, id0, id1 );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoGetIdV4L2( platformGenericVidParam->device.v4l2, id0, id1 );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoGetIdDv( platformGenericVidParam->device.dv, id0, id1 );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoGetId1394( platformGenericVidParam->device.cam1394, id0, id1 );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoGetIdGStreamer( platformGenericVidParam->device.gstreamer, id0, id1 );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoGetIdSGI( platformGenericVidParam->device.sgi, id0, id1 );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoGetIdWinDS( platformGenericVidParam->device.winDS, id0, id1 );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoGetIdWinDSVL( platformGenericVidParam->device.winDSVL, id0, id1 );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoGetIdWinDF( platformGenericVidParam->device.winDF, id0, id1 );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoGetIdQuickTime( platformGenericVidParam->device.quickTime, id0, id1 );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetIdiPhone( platformGenericVidParam->device.iPhone, id0, id1 );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetIdQuickTime7( platformGenericVidParam->device.quickTime7, id0, id1 );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetIdImage( platformGenericVidParam->device.image, id0, id1 );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetIdAndroid( platformGenericVidParam->device.android, id0, id1 );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetIdWinMF( platformGenericVidParam->device.winMF, id0, id1 );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetIdWinMC( platformGenericVidParam->device.winMC, id0, id1 );
    }
#endif
    return (-1);
}

int ar2VideoGetSize(AR2VideoParamT *platformGenericVidParam, int *x,int *y)
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetSizeDummy( platformGenericVidParam->device.dummy, x, y );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoGetSizeV4L( platformGenericVidParam->device.v4l, x, y );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoGetSizeV4L2( platformGenericVidParam->device.v4l2, x, y );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoGetSizeDv( platformGenericVidParam->device.dv, x, y );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoGetSize1394( platformGenericVidParam->device.cam1394, x, y );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoGetSizeGStreamer( platformGenericVidParam->device.gstreamer, x, y );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoGetSizeSGI( platformGenericVidParam->device.sgi, x, y );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoGetSizeWinDS( platformGenericVidParam->device.winDS, x, y );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoGetSizeWinDSVL( platformGenericVidParam->device.winDSVL, x, y );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoGetSizeWinDF( platformGenericVidParam->device.winDF, x, y );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoGetSizeQuickTime( platformGenericVidParam->device.quickTime, x, y );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetSizeiPhone( platformGenericVidParam->device.iPhone, x, y );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetSizeQuickTime7( platformGenericVidParam->device.quickTime7, x, y );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetSizeImage( platformGenericVidParam->device.image, x, y );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetSizeAndroid( platformGenericVidParam->device.android, x, y );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetSizeWinMF( platformGenericVidParam->device.winMF, x, y );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetSizeWinMC( platformGenericVidParam->device.winMC, x, y );
    }
#endif
    return (-1);
}

int ar2VideoGetPixelSize( AR2VideoParamT *platformGenericVidParam )
{
    return (arVideoUtilGetPixelSize(ar2VideoGetPixelFormat(platformGenericVidParam)));
}

AR_PIXEL_FORMAT ar2VideoGetPixelFormat( AR2VideoParamT *platformGenericVidParam )
{
    if (!platformGenericVidParam) return (AR_PIXEL_FORMAT_INVALID);
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetPixelFormatDummy( platformGenericVidParam->device.dummy );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoGetPixelFormatV4L( platformGenericVidParam->device.v4l );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoGetPixelFormatV4L2( platformGenericVidParam->device.v4l2 );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoGetPixelFormatDv( platformGenericVidParam->device.dv );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoGetPixelFormat1394( platformGenericVidParam->device.cam1394 );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoGetPixelFormatGStreamer( platformGenericVidParam->device.gstreamer );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoGetPixelFormatSGI( platformGenericVidParam->device.sgi );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoGetPixelFormatWinDS( platformGenericVidParam->device.winDS );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoGetPixelFormatWinDSVL( platformGenericVidParam->device.winDSVL );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoGetPixelFormatWinDF( platformGenericVidParam->device.winDF );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoGetPixelFormatQuickTime( platformGenericVidParam->device.quickTime );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetPixelFormatiPhone( platformGenericVidParam->device.iPhone );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetPixelFormatQuickTime7( platformGenericVidParam->device.quickTime7 );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetPixelFormatImage( platformGenericVidParam->device.image );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetPixelFormatAndroid( platformGenericVidParam->device.android );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetPixelFormatWinMF( platformGenericVidParam->device.winMF );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetPixelFormatWinMC( platformGenericVidParam->device.winMC );
    }
#endif
    return (AR_PIXEL_FORMAT_INVALID);
}

AR2VideoBufferT *ar2VideoGetImage( AR2VideoParamT *platformGenericVidParam )
{
    if (!platformGenericVidParam) return (NULL);
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetImageDummy( platformGenericVidParam->device.dummy );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoGetImageV4L( platformGenericVidParam->device.v4l );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoGetImageV4L2( platformGenericVidParam->device.v4l2 );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoGetImageDv( platformGenericVidParam->device.dv );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoGetImage1394( platformGenericVidParam->device.cam1394 );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoGetImageGStreamer( platformGenericVidParam->device.gstreamer );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoGetImageSGI( platformGenericVidParam->device.sgi );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoGetImageWinDS( platformGenericVidParam->device.winDS );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoGetImageWinDSVL( platformGenericVidParam->device.winDSVL );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoGetImageWinDF( platformGenericVidParam->device.winDF );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoGetImageQuickTime( platformGenericVidParam->device.quickTime );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetImageiPhone( platformGenericVidParam->device.iPhone );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetImageQuickTime7( platformGenericVidParam->device.quickTime7 );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetImageImage( platformGenericVidParam->device.image );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
#  if AR_VIDEO_ANDROID_ENABLE_NATIVE_CAMERA
        return ar2VideoGetImageAndroid( platformGenericVidParam->device.android );
#  else
        return (NULL); // NOT IMPLEMENTED.
#  endif
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetImageWinMF( platformGenericVidParam->device.winMF );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetImageWinMC( platformGenericVidParam->device.winMC );
    }
#endif
    return (NULL);
}

int ar2VideoCapStart( AR2VideoParamT *platformGenericVidParam )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoCapStartDummy( platformGenericVidParam->device.dummy );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoCapStartV4L( platformGenericVidParam->device.v4l );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoCapStartV4L2( platformGenericVidParam->device.v4l2 );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoCapStartDv( platformGenericVidParam->device.dv );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoCapStart1394( platformGenericVidParam->device.cam1394 );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoCapStartGStreamer( platformGenericVidParam->device.gstreamer );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoCapStartSGI( platformGenericVidParam->device.sgi );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoCapStartWinDS( platformGenericVidParam->device.winDS );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoCapStartWinDSVL( platformGenericVidParam->device.winDSVL );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoCapStartWinDF( platformGenericVidParam->device.winDF );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoCapStartQuickTime( platformGenericVidParam->device.quickTime );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoCapStartiPhone( platformGenericVidParam->device.iPhone );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoCapStartQuickTime7( platformGenericVidParam->device.quickTime7 );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoCapStartImage( platformGenericVidParam->device.image );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
#  if AR_VIDEO_ANDROID_ENABLE_NATIVE_CAMERA
        return ar2VideoCapStartAndroid( platformGenericVidParam->device.android );
#  else
        return (-1); // NOT IMPLEMENTED.
#  endif
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoCapStartWinMF( platformGenericVidParam->device.winMF );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoCapStartWinMC( platformGenericVidParam->device.winMC );
    }
#endif
    return (-1);
}

int ar2VideoCapStartAsync (AR2VideoParamT *platformGenericVidParam, AR_VIDEO_FRAME_READY_CALLBACK callback, void *userdata)
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
#  if AR_VIDEO_ANDROID_ENABLE_NATIVE_CAMERA
        return ar2VideoCapStartAsyncAndroid( platformGenericVidParam->device.android, callback, userdata );
#  else
        return (-1); // NOT IMPLEMENTED.
#  endif
    }
#endif
    return (-1);
}

int ar2VideoCapStop( AR2VideoParamT *platformGenericVidParam )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoCapStopDummy( platformGenericVidParam->device.dummy );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoCapStopV4L( platformGenericVidParam->device.v4l );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoCapStopV4L2( platformGenericVidParam->device.v4l2 );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoCapStopDv( platformGenericVidParam->device.dv );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoCapStop1394( platformGenericVidParam->device.cam1394 );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoCapStopGStreamer( platformGenericVidParam->device.gstreamer );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoCapStopSGI( platformGenericVidParam->device.sgi );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoCapStopWinDS( platformGenericVidParam->device.winDS );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoCapStopWinDSVL( platformGenericVidParam->device.winDSVL );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoCapStopWinDF( platformGenericVidParam->device.winDF );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoCapStopQuickTime( platformGenericVidParam->device.quickTime );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoCapStopiPhone( platformGenericVidParam->device.iPhone );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoCapStopQuickTime7( platformGenericVidParam->device.quickTime7 );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoCapStopImage( platformGenericVidParam->device.image );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
#  if AR_VIDEO_ANDROID_ENABLE_NATIVE_CAMERA
		return ar2VideoCapStopAndroid( platformGenericVidParam->device.android );
#  else
        return (-1); // NOT IMPLEMENTED.
#  endif
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoCapStopWinMF( platformGenericVidParam->device.winMF );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoCapStopWinMC( platformGenericVidParam->device.winMC );
    }
#endif
    return (-1);
}

int ar2VideoGetParami( AR2VideoParamT *platformGenericVidParam, int paramName, int *value )
{
    if (paramName == AR_VIDEO_GET_VERSION) {
#if (AR_HEADER_VERSION_MAJOR >= 10)
        return (-1);
#else
        return (0x01000000 * ((unsigned int)AR_HEADER_VERSION_MAJOR) +
                0x00100000 * ((unsigned int)AR_HEADER_VERSION_MINOR / 10) +
                0x00010000 * ((unsigned int)AR_HEADER_VERSION_MINOR % 10) +
                0x00001000 * ((unsigned int)AR_HEADER_VERSION_TINY / 10) +
                0x00000100 * ((unsigned int)AR_HEADER_VERSION_TINY % 10) +
                0x00000010 * ((unsigned int)AR_HEADER_VERSION_BUILD / 10) +
                0x00000001 * ((unsigned int)AR_HEADER_VERSION_BUILD % 10)
                );
#endif
    }

    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetParamiDummy( platformGenericVidParam->device.dummy, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoGetParamiV4L( platformGenericVidParam->device.v4l, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoGetParamiV4L2( platformGenericVidParam->device.v4l2, paramName, value );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoGetParamiDv( platformGenericVidParam->device.dv, paramName, value );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoGetParami1394( platformGenericVidParam->device.cam1394, paramName, value );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoGetParamiGStreamer( platformGenericVidParam->device.gstreamer, paramName, value );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoGetParamiSGI( platformGenericVidParam->device.sgi, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoGetParamiWinDS( platformGenericVidParam->device.winDS, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoGetParamiWinDSVL( platformGenericVidParam->device.winDSVL, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoGetParamiWinDF( platformGenericVidParam->device.winDF, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoGetParamiQuickTime( platformGenericVidParam->device.quickTime, paramName, value );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetParamiiPhone( platformGenericVidParam->device.iPhone, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetParamiQuickTime7( platformGenericVidParam->device.quickTime7, paramName, value );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetParamiImage( platformGenericVidParam->device.image, paramName, value );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetParamiAndroid( platformGenericVidParam->device.android, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetParamiWinMF( platformGenericVidParam->device.winMF, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetParamiWinMC( platformGenericVidParam->device.winMC, paramName, value );
    }
#endif
    return (-1);
}

int ar2VideoSetParami( AR2VideoParamT *platformGenericVidParam, int paramName, int  value )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoSetParamiDummy( platformGenericVidParam->device.dummy, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoSetParamiV4L( platformGenericVidParam->device.v4l, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoSetParamiV4L2( platformGenericVidParam->device.v4l2, paramName, value );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoSetParamiDv( platformGenericVidParam->device.dv, paramName, value );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoSetParami1394( platformGenericVidParam->device.cam1394, paramName, value );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoSetParamiGStreamer( platformGenericVidParam->device.gstreamer, paramName, value );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoSetParamiSGI( platformGenericVidParam->device.sgi, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoSetParamiWinDS( platformGenericVidParam->device.winDS, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoSetParamiWinDSVL( platformGenericVidParam->device.winDSVL, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoSetParamiWinDF( platformGenericVidParam->device.winDF, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoSetParamiQuickTime( platformGenericVidParam->device.quickTime, paramName, value );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoSetParamiiPhone( platformGenericVidParam->device.iPhone, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoSetParamiQuickTime7( platformGenericVidParam->device.quickTime7, paramName, value );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoSetParamiImage( platformGenericVidParam->device.image, paramName, value );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoSetParamiAndroid( platformGenericVidParam->device.android, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoSetParamiWinMF( platformGenericVidParam->device.winMF, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoSetParamiWinMC( platformGenericVidParam->device.winMC, paramName, value );
    }
#endif
    return (-1);
}

int ar2VideoGetParamd( AR2VideoParamT *platformGenericVidParam, int paramName, double *value )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetParamdDummy( platformGenericVidParam->device.dummy, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoGetParamdV4L( platformGenericVidParam->device.v4l, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoGetParamdV4L2( platformGenericVidParam->device.v4l2, paramName, value );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoGetParamdDv( platformGenericVidParam->device.dv, paramName, value );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoGetParamd1394( platformGenericVidParam->device.cam1394, paramName, value );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoGetParamdGStreamer( platformGenericVidParam->device.gstreamer, paramName, value );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoGetParamdSGI( platformGenericVidParam->device.sgi, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoGetParamdWinDS( platformGenericVidParam->device.winDS, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoGetParamdWinDSVL( platformGenericVidParam->device.winDSVL, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoGetParamdWinDF( platformGenericVidParam->device.winDF, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoGetParamdQuickTime( platformGenericVidParam->device.quickTime, paramName, value );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetParamdiPhone( platformGenericVidParam->device.iPhone, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetParamdQuickTime7( platformGenericVidParam->device.quickTime7, paramName, value );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetParamdImage( platformGenericVidParam->device.image, paramName, value );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetParamdAndroid( platformGenericVidParam->device.android, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetParamdWinMF( platformGenericVidParam->device.winMF, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetParamdWinMC( platformGenericVidParam->device.winMC, paramName, value );
    }
#endif
    return (-1);
}

int ar2VideoSetParamd( AR2VideoParamT *platformGenericVidParam, int paramName, double  value )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoSetParamdDummy( platformGenericVidParam->device.dummy, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L ) {
        return ar2VideoSetParamdV4L( platformGenericVidParam->device.v4l, paramName, value );
    }
#endif
#ifdef AR_INPUT_V4L2
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_V4L2 ) {
        return ar2VideoSetParamdV4L2( platformGenericVidParam->device.v4l2, paramName, value );
    }
#endif
#ifdef AR_INPUT_DV
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DV ) {
        return ar2VideoSetParamdDv( platformGenericVidParam->device.dv, paramName, value );
    }
#endif
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoSetParamd1394( platformGenericVidParam->device.cam1394, paramName, value );
    }
#endif
#ifdef AR_INPUT_GSTREAMER
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_GSTREAMER ) {
        return ar2VideoSetParamdGStreamer( platformGenericVidParam->device.gstreamer, paramName, value );
    }
#endif
#ifdef AR_INPUT_SGI
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_SGI ) {
        return ar2VideoSetParamdSGI( platformGenericVidParam->device.sgi, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DIRECTSHOW
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW ) {
        return ar2VideoSetParamdWinDS( platformGenericVidParam->device.winDS, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DSVIDEOLIB
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB ) {
        return ar2VideoSetParamdWinDSVL( platformGenericVidParam->device.winDSVL, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_DRAGONFLY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY ) {
        return ar2VideoSetParamdWinDF( platformGenericVidParam->device.winDF, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME ) {
        return ar2VideoSetParamdQuickTime( platformGenericVidParam->device.quickTime, paramName, value );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoSetParamdiPhone( platformGenericVidParam->device.iPhone, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoSetParamdQuickTime7( platformGenericVidParam->device.quickTime7, paramName, value );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoSetParamdImage( platformGenericVidParam->device.image, paramName, value );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoSetParamdAndroid( platformGenericVidParam->device.android, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoSetParamdWinMF( platformGenericVidParam->device.winMF, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoSetParamdWinMC( platformGenericVidParam->device.winMC, paramName, value );
    }
#endif
    return (-1);
}


int ar2VideoGetParams( AR2VideoParamT *platformGenericVidParam, const int paramName, char **value )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetParamsDummy( platformGenericVidParam->device.dummy, paramName, value );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetParamsiPhone( platformGenericVidParam->device.iPhone, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoGetParamsQuickTime7(platformGenericVidParam->device.quickTime7, paramName, value );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetParamsImage( platformGenericVidParam->device.image, paramName, value );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetParamsAndroid( platformGenericVidParam->device.android, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoGetParamsWinMF( platformGenericVidParam->device.winMF, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoGetParamsWinMC( platformGenericVidParam->device.winMC, paramName, value );
    }
#endif
    return (-1);
}

int ar2VideoSetParams( AR2VideoParamT *platformGenericVidParam, const int paramName, const char  *value )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoSetParamsDummy( platformGenericVidParam->device.dummy, paramName, value );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoSetParamsiPhone( platformGenericVidParam->device.iPhone, paramName, value );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        return ar2VideoSetParamsQuickTime7( platformGenericVidParam->device.quickTime7, paramName, value );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoSetParamsImage( platformGenericVidParam->device.image, paramName, value );
    }
#endif
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoSetParamsAndroid( platformGenericVidParam->device.android, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_FOUNDATION
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION ) {
        return ar2VideoSetParamsWinMF( platformGenericVidParam->device.winMF, paramName, value );
    }
#endif
#ifdef AR_INPUT_WINDOWS_MEDIA_CAPTURE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE ) {
        return ar2VideoSetParamsWinMC( platformGenericVidParam->device.winMC, paramName, value );
    }
#endif
    return (-1);
}

int ar2VideoSaveParam( AR2VideoParamT *platformGenericVidParam, char *filename )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoSaveParam1394( platformGenericVidParam->device.cam1394, filename );
    }
#endif
    return (-1);
}

int ar2VideoLoadParam( AR2VideoParamT *platformGenericVidParam, char *filename )
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_1394CAM
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_1394CAM ) {
        return ar2VideoLoadParam1394( platformGenericVidParam->device.cam1394, filename );
    }
#endif
    return (-1);
}

int ar2VideoSetBufferSize(AR2VideoParamT *platformGenericVidParam, const int width, const int height)
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoSetBufferSizeDummy( platformGenericVidParam->device.dummy, width, height );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoSetBufferSizeiPhone( platformGenericVidParam->device.iPhone, width, height );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        //return ar2VideoSetBufferSizeQuickTime7( platformGenericVidParam->device.quickTime7, width, height );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoSetBufferSizeImage( platformGenericVidParam->device.image, width, height );
    }
#endif
    return (-1);
}

int ar2VideoGetBufferSize(AR2VideoParamT *platformGenericVidParam, int *width, int *height)
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_DUMMY
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_DUMMY ) {
        return ar2VideoGetBufferSizeDummy( platformGenericVidParam->device.dummy, width, height );
    }
#endif
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetBufferSizeiPhone( platformGenericVidParam->device.iPhone, width, height );
    }
#endif
#ifdef AR_INPUT_QUICKTIME7
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_QUICKTIME7 ) {
        //return ar2VideoGetBufferSizeQuickTime7( platformGenericVidParam->device.quickTime7, width, height );
    }
#endif
#ifdef AR_INPUT_IMAGE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IMAGE ) {
        return ar2VideoGetBufferSizeImage( platformGenericVidParam->device.image, width, height );
    }
#endif
    return (-1);
}

int ar2VideoGetCParam(AR2VideoParamT *platformGenericVidParam, ARParam *cparam)
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_IPHONE
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_IPHONE ) {
        return ar2VideoGetCParamiPhone( platformGenericVidParam->device.iPhone, cparam );
    }
#endif
    return (-1);
}

int ar2VideoGetCParamAsync(AR2VideoParamT *platformGenericVidParam, void (*callback)(const ARParam *, void *), void *userdata)
{
    if (!platformGenericVidParam) return -1;
#ifdef AR_INPUT_ANDROID
    if( platformGenericVidParam->deviceType == AR_VIDEO_DEVICE_ANDROID ) {
        return ar2VideoGetCParamAsyncAndroid( platformGenericVidParam->device.android, callback, userdata);
    }
#endif
    return (-1);
}

