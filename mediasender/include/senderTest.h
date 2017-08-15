#ifndef _SENDERTEST_H
#define _SENDERTEST_H
// senderTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#include "resource.h"		// main symbols


// CsenderTestApp:
// See senderTest.cpp for the implementation of this class
//

class CsenderTestApp : public CWinApp
{
public:
	CsenderTestApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CsenderTestApp theApp;
#endif
