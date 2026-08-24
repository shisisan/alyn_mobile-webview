//
// Created by ALYN on 2024/8/7.
//

#include "rw.h"
#include "Core/Vector.h"

RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera)
{
	return Memory::callFunction<RpWorld*>("_Z16RpWorldAddCameraP7RpWorldP8RwCamera", world, camera);
}

RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light)
{
	return Memory::callFunction<RpWorld*>("_Z15RpWorldAddLightP7RpWorldP7RpLight", world, light);
}

RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light)
{
	return Memory::callFunction<RpWorld*>("_Z18RpWorldRemoveLightP7RpWorldP7RpLight", world, light);
}

void setScissorRect(void* pRect)
{
	return Memory::callFunction<void>("_ZN7CWidget10SetScissorER5CRect", pRect);
}

RwReal getNearScreenZ()
{
	return Memory::callFunction<RwReal>("_Z20RwIm2DGetNearScreenZv");
}

RwReal getRecipNearClip()
{
	return Memory::callFunction<RwReal>("_ZN9CSprite2d16SetRecipNearClipEv");
}

RwCamera* RwCameraCreate()
{
	return Memory::callFunction<RwCamera*>("_Z14RwCameraCreatev");
}

RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* rgba, RwInt32 i)
{
	return Memory::callFunction<RwCamera*>("_Z13RwCameraClearP8RwCameraP6RwRGBAi", camera, rgba, i);
}

RwCamera* RwCameraClone(RwCamera* camera)
{
	return Memory::callFunction<RwCamera*>("_Z13RwCameraCloneP8RwCamera", camera);
}

RwCamera* RwCameraBeginUpdate(RwCamera* camera)
{
	return Memory::callFunction<RwCamera*>("_Z19RwCameraBeginUpdateP8RwCamera", camera);
}

RwCamera* RwCameraEndUpdate(RwCamera* camera)
{
	return Memory::callFunction<RwCamera*>("_Z17RwCameraEndUpdateP8RwCamera", camera);
}

RwCamera* RwCameraShowRaster(RwCamera* camera, void* pDev, RwUInt32 flags)
{
	return Memory::callFunction<RwCamera*>("_Z18RwCameraShowRasterP8RwCameraPvj", camera, pDev, flags);
}

RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal f)
{
	return Memory::callFunction<RwCamera*>("_Z23RwCameraSetFarClipPlaneP8RwCameraf", camera, f);
}

RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal f)
{
	return Memory::callFunction<RwCamera*>("_Z24RwCameraSetNearClipPlaneP8RwCameraf", camera, f);
}

RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* v)
{
	return Memory::callFunction<RwCamera*>("_Z21RwCameraSetViewWindowP8RwCameraPK5RwV2d", camera, v);
}

RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection)
{
	return Memory::callFunction<RwCamera*>("_Z21RwCameraSetProjectionP8RwCamera18RwCameraProjection", camera, projection);
}

RwFrame* RwFrameCreate()
{
	return Memory::callFunction<RwFrame*>("_Z13RwFrameCreatev");
}

RwFrame* RwFrameTranslate(RwFrame* rwFrame, const RwV3d* v, RwOpCombineType type)
{
	return Memory::callFunction<RwFrame*>("_Z16RwFrameTranslateP7RwFramePK5RwV3d15RwOpCombineType", rwFrame, v, type);
}

RwFrame* RwFrameRotate(RwFrame* rwFrame, const RwV3d* v, RwReal rot, RwOpCombineType type)
{
	return Memory::callFunction<RwFrame*>("_Z13RwFrameRotateP7RwFramePK5RwV3df15RwOpCombineType", rwFrame, v, rot, type);
}

RwFrame* RwFrameForAllObjects(RwFrame* rwFrame, void* callBack, void* data)
{
	// RwFrameForAllObjects(RwFrame *,RwObject * (*)(RwObject *,void *),void *)
	return Memory::callFunction<RwFrame*>("_Z20RwFrameForAllObjectsP7RwFramePFP8RwObjectS2_PvES3_", rwFrame, callBack, data);
}

RpLight* RpLightCreate(RwInt32 type)
{
	return Memory::callFunction<RpLight*>("_Z13RpLightCreatei", type);
}

RpLight* RpLightSetColor(RpLight* rpLight, RwRGBAReal const* v)
{
	return Memory::callFunction<RpLight*>("_Z15RpLightSetColorP7RpLightPK10RwRGBAReal", rpLight, v);
}

RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags)
{
	return Memory::callFunction<RwRaster*>("_Z14RwRasterCreateiiii", width, height, depth, flags);
}

RwBool RwRasterDestroy(RwRaster* raster)
{
	return Memory::callFunction<RwBool>("_Z15RwRasterDestroyP8RwRaster", raster);
}

RwRaster* RwRasterGetOffset(RwRaster* raster, RwInt16* xOffset, RwInt16* yOffset)
{
	return Memory::callFunction<RwRaster*>("_Z17RwRasterGetOffsetP8RwRasterPsS1_", raster, xOffset, yOffset);
}

RwInt32 RwRasterGetNumLevels(RwRaster* raster)
{
	return Memory::callFunction<RwInt32>("_Z20RwRasterGetNumLevelsP8RwRaster", raster);
}

RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect)
{
	return Memory::callFunction<RwRaster*>("_Z17RwRasterSubRasterP8RwRasterS0_P6RwRect", subRaster, raster, rect);
}

RwRaster* RwRasterRenderFast(RwRaster* raster, RwInt32 x, RwInt32 y)
{
	return Memory::callFunction<RwRaster*>("_Z18RwRasterRenderFastP8RwRasterii", raster, x, y);
}

RwRaster* RwRasterRender(RwRaster* raster, RwInt32 x, RwInt32 y)
{
	return Memory::callFunction<RwRaster*>("_Z14RwRasterRenderP8RwRasterii", raster, x, y);
}

RwRaster* RwRasterRenderScaled(RwRaster* raster, RwRect* rect)
{
	return Memory::callFunction<RwRaster*>("_Z20RwRasterRenderScaledP8RwRasterP6RwRect", raster, rect);
}

RwRaster* RwRasterPushContext(RwRaster* raster)
{
	return Memory::callFunction<RwRaster*>("_Z19RwRasterPushContextP8RwRaster", raster);
}

RwRaster* RwRasterPopContext()
{
	return Memory::callFunction<RwRaster*>("_Z18RwRasterPopContextv");
}

RwRaster* RwRasterGetCurrentContext()
{
	return Memory::callFunction<RwRaster*>("_Z25RwRasterGetCurrentContextv");
}

RwBool RwRasterClear(RwInt32 pixelValue)
{
	return Memory::callFunction<RwBool>("_Z13RwRasterCleari", pixelValue);
}

RwBool RwRasterClearRect(RwRect* rpRect, RwInt32 pixelValue)
{
	return Memory::callFunction<RwBool>("_Z17RwRasterClearRectP6RwRecti", rpRect, pixelValue);
}

RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags)
{
	return Memory::callFunction<RwRaster*>("_Z18RwRasterShowRasterP8RwRasterPvj", raster, dev, flags);
}

RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 lockMode)
{
	return Memory::callFunction<RwUInt8*>("_Z12RwRasterLockP8RwRasterhi", raster, level, lockMode);
}

RwRaster* RwRasterUnlock(RwRaster* raster)
{
	return Memory::callFunction<RwRaster*>("_Z14RwRasterUnlockP8RwRaster", raster);
}

RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 lockMode)
{
	return Memory::callFunction<RwUInt8*>("_Z19RwRasterLockPaletteP8RwRasteri", raster, lockMode);
}

RwRaster* RwRasterUnlockPalette(RwRaster* raster)
{
	return Memory::callFunction<RwRaster*>("_Z21RwRasterUnlockPaletteP8RwRaster", raster);
}

RwImage* RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth)
{
	return Memory::callFunction<RwImage*>("_Z13RwImageCreateiii", width, height, depth);
}

RwBool RwImageDestroy(RwImage* image)
{
	return Memory::callFunction<RwBool>("_Z14RwImageDestroyP7RwImage", image);
}

RwImage* RwImageAllocatePixels(RwImage* image)
{
	return Memory::callFunction<RwImage*>("_Z21RwImageAllocatePixelsP7RwImage", image);
}

RwImage* RwImageFreePixels(RwImage* image)
{
	return Memory::callFunction<RwImage*>("_Z17RwImageFreePixelsP7RwImage", image);
}

RwImage* RwImageCopy(RwImage* destImage, const RwImage* sourceImage)
{
	return Memory::callFunction<RwImage*>("_Z11RwImageCopyP7RwImagePKS_", destImage, sourceImage);
}

RwImage* RwImageResize(RwImage* image, RwInt32 width, RwInt32 height)
{
	return Memory::callFunction<RwImage*>("_Z13RwImageResizeP7RwImageii", image, width, height);
}

RwImage* RwImageApplyMask(RwImage* image, const RwImage* mask)
{
	return Memory::callFunction<RwImage*>("_Z16RwImageApplyMaskP7RwImagePKS_", image, mask);
}

RwImage* RwImageMakeMask(RwImage* image)
{
	return Memory::callFunction<RwImage*>("_Z15RwImageMakeMaskP7RwImage", image);
}

RwImage* RwImageReadMaskedImage(const RwChar* imageName, const RwChar* maskname)
{
	return Memory::callFunction<RwImage*>("_Z22RwImageReadMaskedImagePKcS0_", imageName, maskname);
}

RwImage* RwImageRead(const RwChar* imageName)
{
	return Memory::callFunction<RwImage*>("_Z11RwImageReadPKc", imageName);
}

RwImage* RwImageWrite(RwImage* image, const RwChar* imageName)
{
	return Memory::callFunction<RwImage*>("_Z12RwImageWriteP7RwImagePKc", image, imageName);
}

RwImage* RwImageSetFromRaster(RwImage* image, RwRaster* raster)
{
	return Memory::callFunction<RwImage*>("_Z20RwImageSetFromRasterP7RwImageP8RwRaster", image, raster);
}

RwRaster* RwRasterSetFromImage(RwRaster* raster, RwImage* image)
{
	return Memory::callFunction<RwRaster*>("_Z20RwRasterSetFromImageP8RwRasterP7RwImage", raster, image);
}

RwRaster* RwRasterRead(const RwChar* filename)
{
	return Memory::callFunction<RwRaster*>("_Z12RwRasterReadPKc", filename);
}

RwRaster* RwRasterReadMaskedRaster(const RwChar* filename, const RwChar* maskname)
{
	return Memory::callFunction<RwRaster*>("_Z24RwRasterReadMaskedRasterPKcS0_", filename, maskname);
}

RwImage* RwImageFindRasterFormat(RwImage* ipImage, RwInt32 nRasterType, RwInt32* npWidth, RwInt32* npHeight, RwInt32* npDepth, RwInt32* npFormat)
{
	return Memory::callFunction<RwImage*>("_Z23RwImageFindRasterFormatP7RwImageiPiS1_S1_S1_", ipImage, nRasterType, npWidth, npHeight, npDepth, npFormat);
}

RwImage* RtPNGImageWrite(RwImage* image, const RwChar* imageName)
{
	return Memory::callFunction<RwImage*>("_Z15RtPNGImageWriteP7RwImagePKc", image, imageName);
}

RwImage* RtPNGImageRead(const RwChar* imageName)
{
	return Memory::callFunction<RwImage*>("_Z14RtPNGImageReadPKc", imageName);
}

RwTexture* RwTextureCreate(RwRaster* raster)
{
	return Memory::callFunction<RwTexture*>("_Z15RwTextureCreateP8RwRaster", raster);
}

RwTexture* RwTextureRead(const RwChar* filename, const RwChar* imageName)
{
	return Memory::callFunction<RwTexture*>("_Z13RwTextureReadPKcS0_", filename, imageName);
}

RwTexture* RwTextureDestroy(RwTexture* rwTexture)
{
	return Memory::callFunction<RwTexture*>("_Z16RwTextureDestroyP9RwTexture", rwTexture);
}

RwReal RwIm2DGetNearScreenZ()
{
	return Memory::callFunction<RwReal>("_Z20RwIm2DGetNearScreenZv");
}

RwReal RwIm2DGetFarScreenZ()
{
	return Memory::callFunction<RwReal>("_Z19RwIm2DGetFarScreenZv");
}

RwBool RwRenderStateGet(RwRenderState state, void* value)
{
	return Memory::callFunction<RwReal>("_Z16RwRenderStateGet13RwRenderStatePv", state, value);
}

RwBool RwRenderStateSet(RwRenderState state, void* value)
{
	return Memory::callFunction<RwReal>("_Z16RwRenderStateSet13RwRenderStatePv", state, value);
}

RwBool RwIm2DRenderLine(RwIm2DVertex* vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2)
{
	return Memory::callFunction<RwReal>("_Z16RwIm2DRenderLineP14RwOpenGLVertexiii", vertices, numVertices, vert1, vert2);
}

RwBool RwIm2DRenderTriangle(RwIm2DVertex* vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3)
{
	return Memory::callFunction<RwReal>("_Z20RwIm2DRenderTriangleP14RwOpenGLVertexiiii", vertices, numVertices, vert1, vert2, vert3);
}

RwBool RwIm2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices)
{
	return Memory::callFunction<RwReal>("_Z21RwIm2DRenderPrimitive15RwPrimitiveTypeP14RwOpenGLVertexi", primType, vertices, numVertices);
}

RwBool RwIm2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices, RwImVertexIndex* indices, RwInt32 numIndices)
{
	return Memory::callFunction<RwBool>("_Z28RwIm2DRenderIndexedPrimitive15RwPrimitiveTypeP14RwOpenGLVertexiPti", primType, vertices, numVertices, indices, numIndices);
}

RwObject* RwObjectHasFrameSetFrame(RwCamera* camera, RwFrame* rwFrame)
{
	return Memory::callFunction<RwObject*>("_Z25_rwObjectHasFrameSetFramePvP7RwFrame", camera, rwFrame);
}


// RW MATRIX

RwMatrix* RwMatrixUpdate(RwMatrix* matrix)
{
	matrix->flags &= 0xFFFDFFFC;
	return matrix;
}

RwBool RwMatrixDestroy(RwMatrix* matrix)
{
	return Memory::callFunction<RwBool>("_Z15RwMatrixDestroyP11RwMatrixTag", matrix);
}
