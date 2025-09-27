// Fill out your copyright notice in the Description page of Project Settings.


#include "VortexMoverCVars.h"
#include "HAL/IConsoleManager.h"

namespace VortexMoverCVars
{
	static TAutoConsoleVariable<int32> CVarVortexInputDebug(
	TEXT("vortex.debug.input"),
	0,
	TEXT("Vortex Input Debugging\n")
	TEXT(" 0: off (default)\n")
	TEXT(" 1: log every frame\n")
	TEXT(" 2: log only on change\n"),
	ECVF_Default);
	
	int32 IsInputDebugEnabled()
	{
		return CVarVortexInputDebug.GetValueOnGameThread();
	}
}

