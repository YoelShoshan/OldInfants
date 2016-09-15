#ifndef YOEL_KEYS_H
#define YOEL_KEYS_H

#pragma once


// This is used so that pressing 'b' will not do on-off-on-off loop
typedef tOneTouchKey 
{
	bool IsPressed;
	bool IsOn;
}


#endif