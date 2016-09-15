#ifndef YOEL_SORT_H
#define YOEL_SORT_H

#pragma once

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

struct tSortInfo 
{ 
	int m_iIndex; 
	UINT m_uiSort; 

	inline bool operator < (const tSortInfo &vbr2) const {return m_uiSort<vbr2.m_uiSort;}; 
	inline bool operator > (const tSortInfo &vbr2) const {return m_uiSort>vbr2.m_uiSort;}; 
	inline bool operator == (const tSortInfo &vbr2) const {return m_uiSort==vbr2.m_uiSort;}; 
	inline tSortInfo& operator = (const tSortInfo &vbr2) {m_iIndex=vbr2.m_iIndex; m_uiSort=vbr2.m_uiSort; return *this;}; 
}; 

template< const int max_stackdepth,class _T>
void QuickSort(_T *array, const int num_elements, const int threshold=50)
{

	int stack[max_stackdepth];
	int stackdepth=0;

	_T pivot;
	_T temp;

	int Li=0;
	int	li=0;
	int Ri=num_elements-1;
	int ri=Ri;
	int	j;

LOOP:

	if ((Ri-Li)>threshold)
	{ 
		j=(Li+Ri)>>1;

		// get median-of-three 
		if (array[Li]>array[j])
		{
			if (array[Li]<array[Ri]) j=Li;
		}
		else if (array[Ri]<array[j]) j=Ri; 

		pivot=array[j];
		while (li<ri)
		{
			while(array[li]<pivot) ++li;
			while(array[ri]>pivot) --ri;
			if (li<=ri)
			{
				temp=array[li];
				array[li++]=array[ri];
				array[ri--]=temp;
			}
		}
		//	if (Ri-li > ri-Li)
		//	{
		if (Li<ri)
		{
			stack[stackdepth++]=li;
			li=Li;
			stack[stackdepth++]=Ri;
			Ri=ri;
			goto LOOP;
		}
		if (li<Ri)
		{
			Li=li;
			ri=Ri;
			goto LOOP;
		}
		/*	}
		else
		{	
		if (li<Ri)
		{
		stack[stackdepth++]=Li;
		Li=li;
		stack[stackdepth++]=ri;
		ri=Ri;
		goto LOOP;
		}
		if (Li<ri)
		{
		li=Li;
		Ri=ri;
		goto LOOP;
		}
		}*/
	}
	if (stackdepth)
	{	
		Ri=ri=stack[--stackdepth];	// get backwards from stack
		Li=li=stack[--stackdepth];
		goto LOOP;
	}

	// finally go through all elements with insertion sort
	for (int i=1;i<num_elements;++i) 
	{
		pivot=array[i];
		j=i;	  
		while(j && array[j-1]>pivot)
		{
			array[j]=array[j-1];
			--j;
		}
		if (i!=j) array[j]=pivot;
	}
}









#endif // YOEL_SORT_H