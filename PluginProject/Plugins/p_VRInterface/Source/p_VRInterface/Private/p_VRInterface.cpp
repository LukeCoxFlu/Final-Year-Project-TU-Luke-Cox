// Copyright Epic Games, Inc. All Rights Reserved.

#include "p_VRInterface.h"

#define LOCTEXT_NAMESPACE "Fp_VRInterfaceModule"

void Fp_VRInterfaceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void Fp_VRInterfaceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Fp_VRInterfaceModule, p_VRInterface)