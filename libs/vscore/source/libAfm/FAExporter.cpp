// FAExporter.cpp: implementation of the FAExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "math.h"
#include "myMatrix.h"
#include "FAExporter.h"

namespace VisageSDK
{

void FAExporter::initCurrentAnim()
{
	int i,j,k;

	for(i=0;i<cad.nifs;i++)
	{
		for(j=0;j<cad.ifsa[i].nv;j++)
			for(k=0;k<3;k++)
				cad.ifsa[i].v[j][k] = iad.ifsa[i].v[j][k];
	}
	for(i=0;i<cad.nt;i++)
	{
		for(k=0;k<4;k++)
		{
			if(k<3)
				cad.ta[i].t[k] = iad.ta[i].t[k];
			cad.ta[i].r[k] = iad.ta[i].r[k];
			if(k<3)
				cad.ta[i].s[k] = iad.ta[i].s[k];
		}
	}
}

void FAExporter::processAnimation()
{
	int i,j,k,fr;
	initCurrentAnim();

	if(!ad)
		return;

	for(i=0;i<cad.nifs;i++)
	{
		for(j=0;j<cad.ifsa[i].nv;j++)
			cad.ifsa[i].key = 0;
	}
	for(i=0;i<cad.nt;i++)
	{
		cad.ta[i].keyT = 0;
		cad.ta[i].keyR = 0;
		cad.ta[i].keyS = 0;
	}
	
	for(i=0;i<cad.nifs;i++)
		for(j=0;j<cad.ifsa[i].nv;j++)
			cad.ifsa[i].keys[j]=0;

	for(fr=0;fr<loadedFrames;fr++)
	{
		for(i=0;i<cad.nifs;i++)
		{
			ad[fr].ifsa[i].key = 0;
			for(j=0;j<cad.ifsa[i].nv;j++)
			{
				int key = 0;
				for(k=0;k<3;k++)
				{
					if(fr==0)
					{
						key = 1;
					}
					if(fr>0 && fr<loadedFrames-1)
					{
						if(fdiff(ad[fr].ifsa[i].v[j][k],ad[fr+1].ifsa[i].v[j][k]))
							key = 1;
					}
					if(fr==loadedFrames-1)
					{
						if(fdiff(ad[fr].ifsa[i].v[j][k],0.0))
							key = 1;
					}
					cad.ifsa[i].v[j][k] += ad[fr].ifsa[i].v[j][k];
				}
				ad[fr].ifsa[i].keys[j] = key;
				if(key)
					cad.ifsa[i].keys[j]++;
				if(key)
					ad[fr].ifsa[i].key++;
			}
			if(ad[fr].ifsa[i].key)
				cad.ifsa[i].key++;
		}
		for(i=0;i<cad.nt;i++)
		{
			int keyT = 0;
			int keyR = 0;
			int keyS = 0;
			for(k=0;k<4;k++)
			{
				if(fr==0)
				{
					keyT = 1;
					keyR = 1;
					keyS = 1;
				}
				if(fr>0 && fr<loadedFrames-1)
				{
					if(k<3)
					{
						if(fdiff(ad[fr].ta[i].t[k],ad[fr+1].ta[i].t[k]))
							keyT = 1;
					}
					if(fdiff(ad[fr].ta[i].r[k],ad[fr+1].ta[i].r[k]))
						keyR = 1;
					if(k<3)
					{
						if(fdiff(ad[fr].ta[i].s[k],ad[fr+1].ta[i].s[k]))
							keyS = 1;
					}
				}
				if(fr==loadedFrames-1)
				{
					if(k<3)
					{
						if(fdiff(ad[fr].ta[i].t[k],0.0))
							keyT = 1;
					}
					if(fdiff(ad[fr].ta[i].r[k],0.0))
						keyR = 1;
					if(k<3)
					{
						if(fdiff(ad[fr].ta[i].s[k],0.0))
							keyS = 1;
					}
				}
				if(k<3)
					cad.ta[i].t[k] += ad[fr].ta[i].t[k];
				cad.ta[i].r[k] += ad[fr].ta[i].r[k];
				if(k<3)
					cad.ta[i].s[k] += ad[fr].ta[i].s[k];
			}
			ad[fr].ta[i].keyT = keyT;
			ad[fr].ta[i].keyR = keyR;
			ad[fr].ta[i].keyS = keyS;
			if(ad[fr].ta[i].keyT)
				cad.ta[i].keyT++;
			if(ad[fr].ta[i].keyR)
				cad.ta[i].keyR++;
			if(ad[fr].ta[i].keyS)
				cad.ta[i].keyS++;
		}
	}
	// if there is only one key frame,it's no animation
	for(i=0;i<cad.nifs;i++)
	{
		if(cad.ifsa[i].key == 1) 
			cad.ifsa[i].key = 0;
		for(j=0;j<cad.ifsa[i].nv;j++)
			if(cad.ifsa[i].keys[j] == 1) 
				cad.ifsa[i].keys[j]=0;
	}
	for(i=0;i<cad.nt;i++)
	{
		if(cad.ta[i].keyT == 1) 
			cad.ta[i].keyT = 0;
		if(cad.ta[i].keyR == 1) 
			cad.ta[i].keyR = 0;
		if(cad.ta[i].keyS == 1) 
			cad.ta[i].keyS = 0;
	}

	// same vertices animated in all frames
	for(i=0;i<cad.nifs;i++)
		cad.ifsa[i].key = 0;
	for(fr=0;fr<loadedFrames;fr++)
	{
		for(i=0;i<cad.nifs;i++)
		{
			if(ad[fr].ifsa[i].key==0)
				continue;
			ad[fr].ifsa[i].key = 0;
			for(j=0;j<cad.ifsa[i].nv;j++)
			{
				ad[fr].ifsa[i].keys[j] = cad.ifsa[i].keys[j];
				if(ad[fr].ifsa[i].keys[j])
					ad[fr].ifsa[i].key++;
			}
			if(ad[fr].ifsa[i].key)
				cad.ifsa[i].key++;
		}
	}
	
	initCurrentAnim();
}

//////////////////////////////////////////////////////////////////////
// initialize traversal
//////////////////////////////////////////////////////////////////////


int FAExporter::initializeTransform(float *t,float *r, float *s, float *so, float *c, float *negso, float *negc, int pass)
{
	int i;
	int split = 0;
	if(so[3]!=1.0) split = 1;
	if(c[0]!=0.0) split = 1;
	if(c[1]!=0.0) split = 1;
	if(c[2]!=0.0) split = 1;
	if(forceSplitTransform) split = 1;

	if(pass == 0 || pass == 1)
	{
		if(split)
			tc+=7;
		else
			tc++;
		return(split);
	}

	cad.ta[tc].split = split;
	iad.ta[tc].split = split;
	if(split)
	{

		// translation
		cad.ta[tc].t[0] = t[0];
		cad.ta[tc].t[1] = t[1];
		cad.ta[tc].t[2] = t[2];
		cad.ta[tc].r[0] = 0.0;
		cad.ta[tc].r[1] = 0.0;
		cad.ta[tc].r[2] = 0.0;
		cad.ta[tc].r[3] = 0.0;
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;

		// center translation
		cad.ta[tc].t[0] = c[0];
		cad.ta[tc].t[1] = c[1];
		cad.ta[tc].t[2] = c[2];
		cad.ta[tc].r[0] = 0.0;
		cad.ta[tc].r[1] = 0.0;
		cad.ta[tc].r[2] = 0.0;
		cad.ta[tc].r[3] = 0.0;
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;
		
		// rotation
		cad.ta[tc].t[0] = 0.0;
		cad.ta[tc].t[1] = 0.0;
		cad.ta[tc].t[2] = 0.0;
		cad.ta[tc].r[0] = r[0];
		cad.ta[tc].r[1] = r[1];
		cad.ta[tc].r[2] = r[2];
		cad.ta[tc].r[3] = r[3];
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;

		// scale orientation
		cad.ta[tc].t[0] = 0.0;
		cad.ta[tc].t[1] = 0.0;
		cad.ta[tc].t[2] = 0.0;
		cad.ta[tc].r[0] = so[0];
		cad.ta[tc].r[1] = so[1];
		cad.ta[tc].r[2] = so[2];
		cad.ta[tc].r[3] = so[3];
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;
		
		// scale
		cad.ta[tc].t[0] = 0.0;
		cad.ta[tc].t[1] = 0.0;
		cad.ta[tc].t[2] = 0.0;
		cad.ta[tc].r[0] = 0.0;
		cad.ta[tc].r[1] = 0.0;
		cad.ta[tc].r[2] = 0.0;
		cad.ta[tc].r[3] = 0.0;
		cad.ta[tc].s[0] = s[0];
		cad.ta[tc].s[1] = s[1];
		cad.ta[tc].s[2] = s[2];
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;
		
		// inverse scale orientation rotation
		cad.ta[tc].t[0] = 0.0;
		cad.ta[tc].t[1] = 0.0;
		cad.ta[tc].t[2] = 0.0;
		cad.ta[tc].r[0] = negso[0];
		cad.ta[tc].r[1] = negso[1];
		cad.ta[tc].r[2] = negso[2];
		cad.ta[tc].r[3] = negso[3];
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;
		
		// inverse center translation
		cad.ta[tc].t[0] = negc[0];
		cad.ta[tc].t[1] = negc[1];
		cad.ta[tc].t[2] = negc[2];
		cad.ta[tc].r[0] = 0.0;
		cad.ta[tc].r[1] = 0.0;
		cad.ta[tc].r[2] = 0.0;
		cad.ta[tc].r[3] = 0.0;
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;
	}
	else
	{
		// translation, rotation, scale
		cad.ta[tc].t[0] = t[0];
		cad.ta[tc].t[1] = t[1];
		cad.ta[tc].t[2] = t[2];
		cad.ta[tc].r[0] = r[0];
		cad.ta[tc].r[1] = r[1];
		cad.ta[tc].r[2] = r[2];
		cad.ta[tc].r[3] = r[3];
		cad.ta[tc].s[0] = s[0];
		cad.ta[tc].s[1] = s[1];
		cad.ta[tc].s[2] = s[2];
		for(i=0;i<3;i++)
		{
			iad.ta[tc].t[i] = cad.ta[tc].t[i];
			iad.ta[tc].r[i] = cad.ta[tc].r[i];
			iad.ta[tc].s[i] = cad.ta[tc].s[i];
		}
		iad.ta[tc].r[3] = cad.ta[tc].r[3];
		tc++;
	}
	return(split);
}

void FAExporter::initializeIndexedFaceSet(int nc, float *coords,int pass)
{
	int i,j;

	if(pass == 0)
	{
		gc++;
		return;
	}

	cad.ifsa[gc].nv = nc;
	iad.ifsa[gc].nv = nc;
	if(pass == 1)
	{
		gc++;
		return;
	}
	for(i=0;i<nc;i++)
		for(j=0;j<3;j++)
			cad.ifsa[gc].v[i][j] = coords[i*3+j];
	for(i=0;i<nc;i++)
		for(j=0;j<3;j++)
			iad.ifsa[gc].v[i][j] = coords[i*3+j];
	
	gc++;
}

//////////////////////////////////////////////////////////////////////
// add frame traversal
//////////////////////////////////////////////////////////////////////


int FAExporter::addFrameTransform(float *t,float *r, float *s, float *so, float *c, float *negso, float *negc)
{
	int split = 0;
	if(so[3]!=1.0) split = 1;
	if(c[0]!=0.0) split = 1;
	if(c[1]!=0.0) split = 1;
	if(c[2]!=0.0) split = 1;
	if(forceSplitTransform) split = 1;
	
	if(split)
	{

		// translation
		ad[loadedFrames].ta[tc].t[0] = t[0] - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = t[1] - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = t[2] - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = 0.0f - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = 0.0f - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = 0.0f - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = 1.0f - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = 1.0f - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = 1.0f - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = 1.0f - cad.ta[tc].s[2];
		cad.ta[tc].t[0] = t[0];
		cad.ta[tc].t[1] = t[1];
		cad.ta[tc].t[2] = t[2];
		cad.ta[tc].r[0] = 0.0f;
		cad.ta[tc].r[1] = 0.0f;
		cad.ta[tc].r[2] = 0.0f;
		cad.ta[tc].r[3] = 1.0f;
		cad.ta[tc].s[0] = 1.0f;
		cad.ta[tc].s[1] = 1.0f;
		cad.ta[tc].s[2] = 1.0f;
		tc++;

		// center translation
		ad[loadedFrames].ta[tc].t[0] = c[0] - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = c[1] - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = c[2] - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = 0.0f - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = 0.0f - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = 0.0f - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = 1.0f - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = 1.0f - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = 1.0f - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = 1.0f - cad.ta[tc].s[2];
		cad.ta[tc].t[0] = c[0];
		cad.ta[tc].t[1] = c[1];
		cad.ta[tc].t[2] = c[2];
		cad.ta[tc].r[0] = 0.0f;
		cad.ta[tc].r[1] = 0.0f;
		cad.ta[tc].r[2] = 0.0f;
		cad.ta[tc].r[3] = 1.0f;
		cad.ta[tc].s[0] = 1.0f;
		cad.ta[tc].s[1] = 1.0f;
		cad.ta[tc].s[2] = 1.0f;
		tc++;
		
		// rotation
		ad[loadedFrames].ta[tc].t[0] = 0.0f - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = 0.0f - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = 0.0f - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = r[0] - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = r[1] - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = r[2] - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = r[3] - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = 1.0f - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = 1.0f - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = 1.0f - cad.ta[tc].s[2];
		cad.ta[tc].t[0] = 0.0f;
		cad.ta[tc].t[1] = 0.0f;
		cad.ta[tc].t[2] = 0.0f;
		cad.ta[tc].r[0] = r[0];
		cad.ta[tc].r[1] = r[1];
		cad.ta[tc].r[2] = r[2];
		cad.ta[tc].r[3] = r[3];
		cad.ta[tc].s[0] = 1.0;
		cad.ta[tc].s[1] = 1.0;
		cad.ta[tc].s[2] = 1.0;
		tc++;

		// scale orientation
		ad[loadedFrames].ta[tc].t[0] = 0.0f - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = 0.0f - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = 0.0f - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = so[0] - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = so[1] - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = so[2] - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = so[3] - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = 1.0f - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = 1.0f - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = 1.0f - cad.ta[tc].s[2];
		cad.ta[tc].t[0] = 0.0f;
		cad.ta[tc].t[1] = 0.0f;
		cad.ta[tc].t[2] = 0.0f;
		cad.ta[tc].r[0] = so[0];
		cad.ta[tc].r[1] = so[1];
		cad.ta[tc].r[2] = so[2];
		cad.ta[tc].r[3] = so[3];
		cad.ta[tc].s[0] = 1.0f;
		cad.ta[tc].s[1] = 1.0f;
		cad.ta[tc].s[2] = 1.0f;
		tc++;
		
		// scale
		ad[loadedFrames].ta[tc].t[0] = 0.0f - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = 0.0f - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = 0.0f - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = 0.0f - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = 0.0f - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = 0.0f - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = 1.0f - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = s[0] - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = s[1] - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = s[2] - cad.ta[tc].s[2];
		cad.ta[tc].t[0] = 0.0f;
		cad.ta[tc].t[1] = 0.0f;
		cad.ta[tc].t[2] = 0.0f;
		cad.ta[tc].r[0] = 0.0f;
		cad.ta[tc].r[1] = 0.0f;
		cad.ta[tc].r[2] = 0.0f;
		cad.ta[tc].r[3] = 1.0f;
		cad.ta[tc].s[0] = s[0];
		cad.ta[tc].s[1] = s[1];
		cad.ta[tc].s[2] = s[2];
		tc++;
		
		// inverse scale orientation rotation
		cad.ta[tc].t[0] = 0.0f;
		cad.ta[tc].t[1] = 0.0f;
		cad.ta[tc].t[2] = 0.0f;
		cad.ta[tc].r[0] = negso[0];
		cad.ta[tc].r[1] = negso[1];
		cad.ta[tc].r[2] = negso[2];
		cad.ta[tc].r[3] = negso[3];
		cad.ta[tc].s[0] = 1.0f;
		cad.ta[tc].s[1] = 1.0f;
		cad.ta[tc].s[2] = 1.0f;
		ad[loadedFrames].ta[tc].t[0] = 0.0f - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = 0.0f - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = 0.0f - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = negso[0] - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = negso[1] - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = negso[2] - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = negso[3] - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = 1.0f - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = 1.0f - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = 1.0f - cad.ta[tc].s[2];
		tc++;
		
		// inverse center translation
		cad.ta[tc].t[0] = negc[0];
		cad.ta[tc].t[1] = negc[1];
		cad.ta[tc].t[2] = negc[2];
		cad.ta[tc].r[0] = 0.0f;
		cad.ta[tc].r[1] = 0.0f;
		cad.ta[tc].r[2] = 0.0f;
		cad.ta[tc].r[3] = 1.0f;
		cad.ta[tc].s[0] = 1.0f;
		cad.ta[tc].s[1] = 1.0f;
		cad.ta[tc].s[2] = 1.0f;
		ad[loadedFrames].ta[tc].t[0] = negc[0] - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = negc[1] - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = negc[2] - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = 0.0f - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = 0.0f - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = 0.0f - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = 1.0f - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = 1.0f - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = 1.0f - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = 1.0f - cad.ta[tc].s[2];
		tc++;
	}
	else
	{
		// translation, rotation, scale
		ad[loadedFrames].ta[tc].t[0] = t[0] - cad.ta[tc].t[0];
		ad[loadedFrames].ta[tc].t[1] = t[1] - cad.ta[tc].t[1];
		ad[loadedFrames].ta[tc].t[2] = t[2] - cad.ta[tc].t[2];
		ad[loadedFrames].ta[tc].r[0] = r[0] - cad.ta[tc].r[0];
		ad[loadedFrames].ta[tc].r[1] = r[1] - cad.ta[tc].r[1];
		ad[loadedFrames].ta[tc].r[2] = r[2] - cad.ta[tc].r[2];
		ad[loadedFrames].ta[tc].r[3] = r[3] - cad.ta[tc].r[3];
		ad[loadedFrames].ta[tc].s[0] = s[0] - cad.ta[tc].s[0];
		ad[loadedFrames].ta[tc].s[1] = s[1] - cad.ta[tc].s[1];
		ad[loadedFrames].ta[tc].s[2] = s[2] - cad.ta[tc].s[2];
		cad.ta[tc].t[0] = t[0];
		cad.ta[tc].t[1] = t[1];
		cad.ta[tc].t[2] = t[2];
		cad.ta[tc].r[0] = r[0];
		cad.ta[tc].r[1] = r[1];
		cad.ta[tc].r[2] = r[2];
		cad.ta[tc].r[3] = r[3];
		cad.ta[tc].s[0] = s[0];
		cad.ta[tc].s[1] = s[1];
		cad.ta[tc].s[2] = s[2];
		tc++;
	}
	return(split);
}

void FAExporter::addFrameIndexedFaceSet(int nc, float *coords)
{
	int i,j;

	for(i=0;i<nc;i++)
		for(j=0;j<3;j++)
			ad[loadedFrames].ifsa[gc].v[i][j] = coords[i*3+j] - cad.ifsa[gc].v[i][j];
	for(i=0;i<nc;i++)
		for(j=0;j<3;j++)
			cad.ifsa[gc].v[i][j] = coords[i*3+j];
	
	gc++;
}


//////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////

void FAExporter::render(VRMLModel *faceModel,long timeMs)
{
	addFrame(faceModel);
}

void FAExporter::preRender(FBAPs *fbaps,long timeMs)
{
}

void FAExporter::addFrame(VRMLModel *m)
{
	if(loadedFrames == nFrames)
		return;
	tc = 0;
	gc = 0;
	traverseType = TT_ADDFRAME;
	m->applySkinning();
	m->applyMorphing();
	m->traverse(m->rootNode,this);
	loadedFrames++;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void FAExporter::init(VRMLModel *m, int nfr)
{
	int i,fr;


	if(m)
	faceModel = new VRMLModel(m);
	else
	{
		faceModel = 0;
		return;
	}
	nFrames = nfr;
	loadedFrames = 0;
	if(nFrames)
		ad = (animData*) malloc(sizeof(animData)*nFrames);
	else
		ad = 0;

	// first initialization pass
	tc = 0;
	gc = 0;
	traverseType = TT_INITIALIZE0;
	m->traverse(m->rootNode,this);
	for(i=0;i<nFrames;i++)
	{
		ad[i].nifs = gc;
		ad[i].nt = tc;
		ad[i].ifsa = 0;
		ad[i].ta = 0;
		if(ad[i].nifs)
			ad[i].ifsa = (ifsAnimData*)malloc(sizeof(ifsAnimData)*ad[i].nifs);
		if(ad[i].nt)
			ad[i].ta = (transformAnimData*)malloc(sizeof(transformAnimData)*ad[i].nt);		
	}
	cad.nifs = gc;
	cad.nt = tc;
	cad.ta = 0;
	cad.ifsa = 0;
	if(cad.nifs)
		cad.ifsa = (ifsAnimData*)malloc(sizeof(ifsAnimData)*cad.nifs);
	if(cad.nt)
		cad.ta = (transformAnimData*)malloc(sizeof(transformAnimData)*cad.nt);		

	iad.nifs = gc;
	iad.nt = tc;
	iad.ta = 0;
	iad.ifsa = 0;
	if(iad.nifs)
		iad.ifsa = (ifsAnimData*)malloc(sizeof(ifsAnimData)*cad.nifs);
	if(iad.nt)
		iad.ta = (transformAnimData*)malloc(sizeof(transformAnimData)*cad.nt);		

	// second initialization pass
	tc = 0;
	gc = 0;
	traverseType = TT_INITIALIZE1;
	m->traverse(m->rootNode,this);
	for(fr=0;fr<nFrames;fr++)
	{
		for(i=0;i<cad.nifs;i++)
		{
			ad[fr].ifsa[i].v = 0;
			ad[fr].ifsa[i].keys = 0;
			if(cad.ifsa[i].nv)
			{
				ad[fr].ifsa[i].v = (myVector*)malloc(sizeof(myVector)*cad.ifsa[i].nv);
				ad[fr].ifsa[i].keys = (int*)malloc(sizeof(int)*cad.ifsa[i].nv);
			}
		}
	}
	for(i=0;i<cad.nifs;i++)
	{
		cad.ifsa[i].v = 0;
		cad.ifsa[i].keys = 0;
		if(cad.ifsa[i].nv)
		{
			cad.ifsa[i].v = (myVector*)malloc(sizeof(myVector)*cad.ifsa[i].nv);
			cad.ifsa[i].keys = (int*)malloc(sizeof(int)*cad.ifsa[i].nv);
		}
		iad.ifsa[i].v = 0;
		iad.ifsa[i].keys = 0;
		if(cad.ifsa[i].nv)
		{
			iad.ifsa[i].v = (myVector*)malloc(sizeof(myVector)*cad.ifsa[i].nv);
			iad.ifsa[i].keys = (int*)malloc(sizeof(int)*cad.ifsa[i].nv);
		}
	}

	// third initialization pass
	tc = 0;
	gc = 0;
	traverseType = TT_INITIALIZE2;
	m->traverse(m->rootNode,this);
}

FAExporter::FAExporter(VRMLModel *m)
{
	forceSplitTransform = 1;
	init(m,0);
}

FAExporter::FAExporter(int forceSplitTransform,VRMLModel *m)
{
	this->forceSplitTransform = forceSplitTransform;
	init(m,0);
}

FAExporter::FAExporter()
{
	forceSplitTransform = 1;
	init(0,0);
}

FAExporter::FAExporter(VRMLModel *m, int nfr)
{
	forceSplitTransform = 1;
	init(m,nfr);
}

FAExporter::FAExporter(int forceSplitTransform,VRMLModel *m, int nfr)
{
	this->forceSplitTransform = forceSplitTransform;
	init(m,nfr);
}

FAExporter::~FAExporter()
{
	int i,j;

	if(faceModel)
		delete faceModel;

	for(j=0;j<cad.nifs;j++)
	{
		if(cad.ifsa[j].v)
			free(cad.ifsa[j].v);
		if(cad.ifsa[j].keys)
			free(cad.ifsa[j].keys);
	}
	if(cad.ifsa)
		free(cad.ifsa);
	if(cad.ta)
		free(cad.ta);
	
	for(j=0;j<iad.nifs;j++)
	{
		if(iad.ifsa[j].v)
			free(iad.ifsa[j].v);
		if(iad.ifsa[j].keys)
			free(iad.ifsa[j].keys);
	}
	if(iad.ifsa)
		free(iad.ifsa);
	
	if(iad.ta)
		free(iad.ta);
	
	if(ad)
	{
		for(i=0;i<nFrames;i++)
		{
			for(j=0;j<ad[i].nifs;j++)
			{
				if(ad[i].ifsa[j].v)
					free(ad[i].ifsa[j].v);
				if(ad[i].ifsa[j].keys)
					free(ad[i].ifsa[j].keys);
			}
			if(ad[i].ifsa)
				free(ad[i].ifsa);
			if(ad[i].ta)
				free(ad[i].ta);
			
		}
		free(ad);
	}
}

// VRMLTraverser callback functions	
void FAExporter::traverseRoot(void *model, int index)
{
}

void FAExporter::traverseRootEnd(void *model, int index)
{
}

int FAExporter::traverseTransform(void *model,    // VRMLModel pointer to the face model
								  int index,      // index of the transform within the face model
								  float *c,       // Center of rotation expressed as translation (x,y,z)
								  float *negc,    // Negative translation of the center of rotation
								  float *r,       // Rotation expressed as axis and angle in radians (x,y,z,angle)
								  float *rq,      // Rotation expressed as quaternion (x,y,z,w)
								  float *s,       // Scale (x,y,z)
								  float *so,      // Scale orientation (acis and angle)
								  float *soq,     // Scale orientation (quaternion)
								  float *negso,   // Inverse of scale orientation (axis and angle)
								  float *negsoq,  // Inverse of scale orientation (quaternion)
								  float *t,       // Translation (x,y,z)
								  myMatrix m)  // Complete transformation matrix
{
	switch(traverseType)
	{
	case TT_INITIALIZE0:
		initializeTransform(t,rq, s, soq, c, negsoq, negc, 0);
		break;
	case TT_INITIALIZE1:
		initializeTransform(t,rq, s, soq, c, negsoq, negc,1);
		break;
	case TT_INITIALIZE2:
		initializeTransform(t,rq, s, soq, c, negsoq, negc,2);
		break;
	case TT_ADDFRAME:
		addFrameTransform(t,rq, s, soq, c, negsoq, negc);
		break;
	};
	return(0);
}

void FAExporter::traverseTransformEnd(void *model, int index)
{
}

void FAExporter::traverseGroup(void *model, int index)
{
}

void FAExporter::traverseGroupEnd(void *model, int index)
{
}

void FAExporter::traverseShape(void *model,         // VRMLModel pointer to the face model
							   int index,           // index of the indexed face set within the face model
							   VRMLMaterial *mat,   // material for this shape
							   float *coord,        // vertex coordinates
							   int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
							   int *coordInd,       // vertex indices for triangles
							   int coordIndLength,  // length of coordInd array
							   float *norm,         // normals
							   int normLength,      // length of norm array
							   int *normInd,        // normal indices, per vertex
							   int normIndLength,   // length of normInd array
							   float *tcoord,       // texture coordinates, if any (else zero)
							   int tcoordLength, 
							   int *tcoordInd, 
							   int tcoordIndLength, 
							   float *cpv,          // colors per vertex, if any (else zero)
							   int cpvLength, 
							   int *cpvInd, 
							   int cpvIndLength,
							   int CPV)            // color per vertex (1) or per face (1)
{
	switch(traverseType)
	{
	case TT_INITIALIZE0:
		initializeIndexedFaceSet(coordLength/3,coord,0);
		break;
	case TT_INITIALIZE1:
		initializeIndexedFaceSet(coordLength/3,coord,1);
		break;
	case TT_INITIALIZE2:
		initializeIndexedFaceSet(coordLength/3,coord,2);
		break;
	case TT_ADDFRAME:
		addFrameIndexedFaceSet(coordLength/3,coord);
		break;
	}
}

void FAExporter::traverseOther(void *model, int index,VRMLNode *node)
{
}

}
