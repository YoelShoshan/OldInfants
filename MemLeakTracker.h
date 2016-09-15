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

#ifndef YOEL_MEM_LEAK_TRACKER
#define YOEL_MEM_LEAK_TRACKER

#pragma once

/////////////////////////////////////////////////////////////////////////////////
// CMemoryLeakFinder - The main class used by ZedTracker (C) 
//                     Those are the parts of the class that i share with you ;)
/////////////////////////////////////////////////////////////////////////////////

class __declspec(dllimport) CMemoryLeakFinder
{
public:

	/////////////////////////////////////////////////////////////
	// CMemoryLeakFinder()
	// *******************
	// Construcer.
	/////////////////////////////////////////////////////////////

	CMemoryLeakFinder();

	/////////////////////////////////////////////////////////////
	// ~CMemoryLeakFinder()
	// *******************
	// Destructer.
	/////////////////////////////////////////////////////////////

	~CMemoryLeakFinder();

	/////////////////////////////////////////////////////////////
	// AddAlloc()
	// **********
	// Add an allocation to the map.
	/////////////////////////////////////////////////////////////
	

	void AddAlloc(void* addr, unsigned int asize, const char* fname, unsigned int lnum);

	/////////////////////////////////////////////////////////////
	// RemoveAlloc()
	// *************
	// Removes an allocation from the map
	/////////////////////////////////////////////////////////////

	void RemoveAlloc(void* addr);


	/////////////////////////////////////////////////////////////
	// IsMapValid()
	// ***********
	// Checks if the std::map is valid
	/////////////////////////////////////////////////////////////

	bool IsMapValid(void);

};

extern __declspec(dllimport) CMemoryLeakFinder g_MemoryLeakFinder;

#endif //YOEL_MEM_LEAK_TRACKER