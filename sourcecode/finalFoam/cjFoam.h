#ifndef _cjFoam_
#define _cjFoam_

#include <maya/MPxLocatorNode.h>
#include <maya/MFloatPoint.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include "distort.h"
#include "noise.h"

class cjFoam : public MPxLocatorNode
{
	public:
						cjFoam();
		virtual         ~cjFoam();

		virtual MStatus compute( const MPlug&, MDataBlock& );
		virtual void    postConstructor();

		void            draw(M3dView &view, 
							 const MDagPath &path, 
							 M3dView::DisplayStyle style, 
							 M3dView::DisplayStatus);
		bool  isBounded();

		float  getColor( const MFloatPoint &spt, //采样点
						  const MVectorArray& posAry,//粒子位置数据
						  const MDoubleArray& radius,//粒子泡沫半径数据
						  short ps,//泡沫形状圆形或正六边形
						  float sp,//抗锯齿
						  float spr);//采样半径
		
		static  void *  creator();
		static  MStatus initialize();

		//  Id tag for use with binary file format
		static  MTypeId id;

	public:

		// Input attributes
		static MObject  foamType;
		static MObject  aUVCoord;
		static MObject  posArray;
		static MObject  radiusArray;
		
		static MObject  baseBound;
		static MObject  sampleR;
		static MObject  samplePercent;

		static MObject  turbulence;
		static MObject  turFrequency;
		static MObject  turPower;
		static MObject  turRoughness;
		static MObject  turSeed;

		static MObject  currentTime;
		static MObject  startTime;

		// Output attributes
		static MObject  aOutColor;
		static MObject  aOutAlpha;		
};
#endif