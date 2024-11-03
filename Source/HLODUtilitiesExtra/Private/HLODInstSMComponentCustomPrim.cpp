// Copyright 2024 NanceDevDiaries. All Rights Reserved.


#include "HLODInstSMComponentCustomPrim.h"

#include "Serialization/ArchiveCrc32.h"

UHLODInstSMComponentCustomPrim::UHLODInstSMComponentCustomPrim(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR

TUniquePtr<FISMComponentDescriptor> UHLODInstSMComponentCustomPrim::AllocateISMComponentDescriptor() const
{
	return MakeUnique<FHLODISMCustomPrimComponentDescriptor>();
}

uint32 FHLODISMCustomPrimComponentDescriptor::ComputeHash() const
{
	Super::ComputeHash();

	FHLODISMCustomPrimComponentDescriptor& MutableSelf = *const_cast<FHLODISMCustomPrimComponentDescriptor*>(this);
	FArchiveCrc32 CrcArchive(Hash);
	CrcArchive << MutableSelf.DefaultCustomPrimData;
	Hash = CrcArchive.GetCrc();
	
	return Hash;
}

FHLODISMCustomPrimComponentDescriptor::FHLODISMCustomPrimComponentDescriptor()
{
	ComponentClass = UHLODInstSMComponentCustomPrim::StaticClass();
}

void FHLODISMCustomPrimComponentDescriptor::InitComponent(UInstancedStaticMeshComponent* ISMComponent) const
{
	for (int32 ColorIndex = 0; ColorIndex < DefaultCustomPrimData.Data.Num(); ColorIndex++)
	{
		ISMComponent->SetDefaultCustomPrimitiveDataFloat(ColorIndex, DefaultCustomPrimData.Data[ColorIndex]);
	}
	
	FHLODISMComponentDescriptor::InitComponent(ISMComponent);
}

void FHLODISMCustomPrimComponentDescriptor::InitFrom(const UStaticMeshComponent* Template, bool bInitBodyInstance)
{
	DefaultCustomPrimData = Template->GetDefaultCustomPrimitiveData();

	Super::InitFrom(Template, bInitBodyInstance);
}

#endif