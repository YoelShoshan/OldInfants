//ZedTracker    Memory Leak Tracker
//Copyright (C) 2004  Yoel Shoshan (zedus@012.net.il)

//This library is free software; you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation; either
//version 2.1 of the License, or (at your option) any later version.

//This library is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.

//You should have received a copy of the GNU Lesser General Public
//License along with this library; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef YOEL_ZEDTRACKER_H
#define YOEL_ZEDTRACKER_H

#pragma once

#include <malloc.h>
#include "MemLeakTracker.h"

#ifdef _DEBUG
#pragma comment( lib, "ZedTracker.lib" )

#pragma warning (disable :4291 )

#pragma warning( push )
#pragma warning (disable :4311 )

#ifdef new
#undef new
#endif


inline void * __cdecl operator new(unsigned int iSize, const char *pcFile, int iLine)
{
	void *ptr = (void *) malloc(iSize);

	g_MemoryLeakFinder.AddAlloc(ptr, iSize, pcFile, iLine);

	return(ptr);
};

inline void * __cdecl operator new[](unsigned int iSize, const char *pcFile, int iLine)
{
	void *ptr = (void *) malloc(iSize);
	
	g_MemoryLeakFinder.AddAlloc(ptr, iSize, pcFile, iLine);

	return(ptr);
};

inline void __cdecl operator delete(void *p)
{
	if (g_MemoryLeakFinder.IsMapValid())
			g_MemoryLeakFinder.RemoveAlloc(p); 

	free(p);
};

inline void __cdecl operator delete[](void *p)
{
	if (g_MemoryLeakFinder.IsMapValid())
			g_MemoryLeakFinder.RemoveAlloc(p); 

	free(p);
};

#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW

#pragma warning( pop )

#endif // _DEBUG

#endif //YOEL_ZEDTRACKER_H