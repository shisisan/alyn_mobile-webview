#pragma once

#include "rwplcore.h"
#include "rwcore.h"
#include "rpworld.h"
#include "Memory.h"
#include "skeleton.h"

//rwcore.h

RwCamera* RwCameraCreate();
RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* rgba, RwInt32 i);
RwCamera* RwCameraClone(RwCamera* camera);
RwCamera* RwCameraBeginUpdate(RwCamera* camera);
RwCamera* RwCameraEndUpdate(RwCamera* camera);
RwCamera* RwCameraShowRaster(RwCamera* camera, void* pDev, RwUInt32 flags);
RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal f);
RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal f);
RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* v);
RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection);

RwFrame* RwFrameCreate();
RwFrame* RwFrameTranslate(RwFrame* rwFrame, const RwV3d* v, RwOpCombineType type);
RwFrame* RwFrameRotate(RwFrame* rwFrame, const RwV3d* v, RwReal rot, RwOpCombineType type);
RwFrame* RwFrameForAllObjects(RwFrame* rwFrame, void* callBack, void* data);

RpLight* RpLightCreate(RwInt32 type);
RpLight* RpLightSetColor(RpLight* rpLight, RwRGBAReal const* v);

RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool RwRasterDestroy(RwRaster* raster);
RwRaster* RwRasterGetOffset(RwRaster* raster, RwInt16* xOffset, RwInt16* yOffset);
RwInt32 RwRasterGetNumLevels(RwRaster* raster);
RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect);
RwRaster* RwRasterRenderFast(RwRaster* raster, RwInt32 x, RwInt32 y);
RwRaster* RwRasterRender(RwRaster* raster, RwInt32 x, RwInt32 y);
RwRaster* RwRasterRenderScaled(RwRaster* raster, RwRect* rect);
RwRaster* RwRasterPushContext(RwRaster* raster);
RwRaster* RwRasterPopContext();
RwRaster* RwRasterGetCurrentContext();
RwBool RwRasterClear(RwInt32 pixelValue);
RwBool RwRasterClearRect(RwRect* rpRect, RwInt32 pixelValue);
RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags);
RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 lockMode);
RwRaster* RwRasterUnlock(RwRaster* raster);
RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 lockMode);
RwRaster* RwRasterUnlockPalette(RwRaster* raster);
RwImage* RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool RwImageDestroy(RwImage* image);
RwImage* RwImageAllocatePixels(RwImage* image);
RwImage* RwImageFreePixels(RwImage* image);
RwImage* RwImageCopy(RwImage* destImage, const RwImage* sourceImage);
RwImage* RwImageResize(RwImage* image, RwInt32 width, RwInt32 height);
RwImage* RwImageApplyMask(RwImage* image, const RwImage* mask);
RwImage* RwImageMakeMask(RwImage* image);
RwImage* RwImageReadMaskedImage(const RwChar* imageName, const RwChar* maskname);
RwImage* RwImageRead(const RwChar* imageName);
RwImage* RwImageWrite(RwImage* image, const RwChar* imageName);
RwImage* RwImageSetFromRaster(RwImage* image, RwRaster* raster);
RwRaster* RwRasterSetFromImage(RwRaster* raster, RwImage* image);
RwRaster* RwRasterRead(const RwChar* filename);
RwRaster* RwRasterReadMaskedRaster(const RwChar* filename, const RwChar* maskname);
RwImage* RwImageFindRasterFormat(RwImage* ipImage, RwInt32 nRasterType, RwInt32* npWidth, RwInt32* npHeight, RwInt32* npDepth, RwInt32* npFormat);

/* rtpng.h */
RwImage* RtPNGImageWrite(RwImage* image, const RwChar* imageName);
RwImage* RtPNGImageRead(const RwChar* imageName);

RwTexture* RwTextureCreate(RwRaster* raster);
RwTexture* RwTextureRead(const RwChar* filename, const RwChar* imageName);
RwTexture* RwTextureDestroy(RwTexture* rwTexture);

void setScissorRect(void* pRect);
RwReal getNearScreenZ();
RwReal getRecipNearClip();




// rwplcore.h

RwReal RwIm2DGetNearScreenZ();
RwReal RwIm2DGetFarScreenZ();
RwBool RwRenderStateGet(RwRenderState state, void* value);
RwBool RwRenderStateSet(RwRenderState state, void* value);
RwBool RwIm2DRenderLine(RwIm2DVertex* vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool RwIm2DRenderTriangle(RwIm2DVertex* vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3);
RwBool RwIm2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices);
RwBool RwIm2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices, RwImVertexIndex* indices, RwInt32 numIndices);

RwObject* RwObjectHasFrameSetFrame(RwCamera* camera, RwFrame* rwFrame);

// rpworld.h
RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera);
RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light);
RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light);

// rw matrix
RwMatrix* RwMatrixUpdate(RwMatrix* matrix);
RwBool RwMatrixDestroy(RwMatrix* matrix);
