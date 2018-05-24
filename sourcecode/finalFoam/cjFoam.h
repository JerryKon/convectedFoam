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

		float  getColor( const MFloatPoint &spt, //������
						  const MVectorArray& posAry,//����λ������
						  const MDoubleArray& radius,//������ĭ�뾶����
						  short ps,//��ĭ��״Բ�λ���������
						  float sp,//�����
						  float spr);//�����뾶
		
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