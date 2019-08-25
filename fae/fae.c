#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/paramGL.h>
#include <AR/gsub_lite.h>
#include <ARUtil/time.h>


static ARHandle		*gARHandle = NULL;
static AR3DHandle	*gAR3DHandle = NULL;
static ARParamLT    *gCparamLT = NULL;

void init()
{
    ARParam			cparam;
    int				xsize, ysize;
    AR_PIXEL_FORMAT pixFormat;

    // Open the video path.
    if (arVideoOpen("") < 0) {
        ARLOGe("setupCamera(): Unable to open connection to camera.\n");
        return;
    }

    // Find the size of the window.
    if (arVideoGetSize(&xsize, &ysize) < 0) {
        ARLOGe("setupCamera(): Unable to determine camera frame size.\n");
        arVideoClose();
        return;
    }
    ARLOGi("Camera image size (x,y) = (%d,%d)\n", xsize, ysize);

    // Get the format in which the camera is returning pixels.
    pixFormat = arVideoGetPixelFormat();
    if (pixFormat == AR_PIXEL_FORMAT_INVALID) {
        ARLOGe("setupCamera(): Camera is using unsupported pixel format.\n");
        arVideoClose();
        return;
    }

    // Load the camera parameters, resize for the window and init.
    arParamClearWithFOVy(&cparam, xsize, ysize, M_PI_4); // M_PI_4 radians = 45 degrees.
    ARLOGw("Using default camera parameters for %dx%d image size, 45 degrees vertical field-of-view.\n", xsize, ysize);
    if (cparam.xsize != xsize || cparam.ysize != ysize) {
        ARLOGw("*** Camera Parameter resized from %d, %d. ***\n", cparam.xsize, cparam.ysize);
        arParamChangeSize(&cparam, xsize, ysize, &cparam);
    }
    if ((gCparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL) {
        ARLOGe("setupCamera(): Error: arParamLTCreate.\n");
        return;
    }

    if ((gARHandle = arCreateHandle(gCparamLT)) == NULL) {
        ARLOGe("setupCamera(): Error: arCreateHandle.\n");
        return;
    }
    if (arSetPixelFormat(gARHandle, pixFormat) < 0) {
        ARLOGe("setupCamera(): Error: arSetPixelFormat.\n");
        return;
    }
    if (arSetDebugMode(gARHandle, AR_DEBUG_DISABLE) < 0) {
        ARLOGe("setupCamera(): Error: arSetDebugMode.\n");
        return;
    }
    if ((gAR3DHandle = ar3DCreateHandle(&cparam)) == NULL) {
        ARLOGe("setupCamera(): Error: ar3DCreateHandle.\n");
        return;
    }

    if (arVideoCapStart() != 0) {
        ARLOGe("setupCamera(): Unable to begin camera data capture.\n");
        return;
    }

    return;
}

int main(){
    arUtilChangeToResourcesDirectory(AR_UTIL_RESOURCES_DIRECTORY_BEHAVIOR_BEST, NULL);
    init();
    arSetLabelingThreshMode(gARHandle, AR_LABELING_THRESH_MODE_AUTO_ADAPTIVE);

}
