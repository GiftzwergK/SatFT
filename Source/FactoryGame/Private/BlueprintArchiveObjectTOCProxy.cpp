// This file has been automatically generated by the Unreal Header Implementation tool

#include "BlueprintArchiveObjectTOCProxy.h"

FBlueprintArchiveObjectTOCProxy::FBlueprintArchiveObjectTOCProxy(FArchive& inInnerArchive,  UWorld* world, FTransform newOrigin) : FArchiveProxy(inInnerArchive) { }
FArchive& FBlueprintArchiveObjectTOCProxy::operator<<( UObject*& Res){ return *(new FArchive); }
