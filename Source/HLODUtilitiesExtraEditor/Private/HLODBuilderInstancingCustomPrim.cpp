// Copyright 2024 NanceDevDiaries. All Rights Reserved.

#include "HLODBuilderInstancingCustomPrim.h"

#include "HLODInstSMComponentCustomPrim.h"
#include "ISMPartition/ISMComponentBatcher.h"

#include "Serialization/ArchiveCrc32.h"

UHLODBuilderInstancingCustomSettings::UHLODBuilderInstancingCustomSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bDisallowWorldPositionOffset(false)
{
}

uint32 UHLODBuilderInstancingCustomSettings::GetCRC() const
{
	UHLODBuilderInstancingCustomSettings& This = *const_cast<UHLODBuilderInstancingCustomSettings*>(this);

	FArchiveCrc32 Ar;

	// Base key, changing this will force a rebuild of all HLODs from this builder
	FString HLODBaseKey = "53809597CD9C4FB7AC75827A628513D6";
	Ar << HLODBaseKey;

	Ar << This.bDisallowNanite;
	UE_LOG(LogHLODBuilder, VeryVerbose, TEXT(" - bDisallowNanite = %d"), Ar.GetCrc());
	Ar << This.bDisallowWorldPositionOffset;
	UE_LOG(LogHLODBuilder, VeryVerbose, TEXT(" - bDisallowedWorldPositionOffset = %d"), Ar.GetCrc());
	
	return Ar.GetCrc();
}

UHLODBuilderInstancingCustomPrim::UHLODBuilderInstancingCustomPrim(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSubclassOf<UHLODBuilderSettings> UHLODBuilderInstancingCustomPrim::GetSettingsClass() const
{
	return UHLODBuilderInstancingCustomSettings::StaticClass();
}


TArray<UActorComponent*> UHLODBuilderInstancingCustomPrim::Build(const FHLODBuildContext& InHLODBuildContext,
                                                                 const TArray<UActorComponent*>& InSourceComponents) const
{
	const UHLODBuilderInstancingCustomSettings& InstancingSettings = *CastChecked<UHLODBuilderInstancingCustomSettings>(HLODBuilderSettings);

	TArray<UActorComponent*> HLODComponents = BatchInstances(InSourceComponents);

	// If requested, disallow Nanite on components whose mesh is Nanite enabled and has multiple LODs
	if (InstancingSettings.bDisallowNanite || InstancingSettings.bDisallowWorldPositionOffset)
	{
		for (UActorComponent* HLODComponent : HLODComponents)
		{
			if (UStaticMeshComponent* SMComponent = Cast<UStaticMeshComponent>(HLODComponent))
			{
				if (InstancingSettings.bDisallowWorldPositionOffset)
				{
					SMComponent->SetEvaluateWorldPositionOffset(false);
				}
				if (InstancingSettings.bDisallowNanite)
				{
					if (const UStaticMesh* StaticMesh = SMComponent->GetStaticMesh())
					{
						if (!SMComponent->bDisallowNanite && StaticMesh->HasValidNaniteData() && StaticMesh->GetNumLODs() > 1)
						{
							SMComponent->bDisallowNanite = true;
							SMComponent->MarkRenderStateDirty();
						}
					}
				}
			}
		}
	}
	
	return HLODComponents;
}

namespace
{
	struct FCustomPrimInstanceBatch
	{
		TUniquePtr<FISMComponentDescriptor>						ISMComponentDescriptor;
		FISMComponentBatcher									ISMComponentBatcher;
	};
}

TArray<UActorComponent*> UHLODBuilderInstancingCustomPrim::BatchInstances(
	const TArray<UActorComponent*>& InSourceComponents)
{
	TArray<UStaticMeshComponent*> SourceStaticMeshComponents = FilterComponents<UStaticMeshComponent>(InSourceComponents);

	// The whole reason we can't just set the ComponentClass :(
	TSubclassOf<UHLODInstancedStaticMeshComponent> ComponentClass =  UHLODInstSMComponentCustomPrim::StaticClass();
	// instead of this function which returns a variable that's not overridable. #TODO remove all this logic if we figure out config var for @HLODInstancedStaticMeshComponentClass
	// TSubclassOf<UHLODInstancedStaticMeshComponent> ComponentClass = UHLODBuilder::GetInstancedStaticMeshComponentClass();
	
	// Prepare instance batches
	TMap<uint32, FCustomPrimInstanceBatch> InstancesData;
	for (const UStaticMeshComponent* SMC : SourceStaticMeshComponents)
	{
		const UStaticMesh* StaticMesh = SMC->GetStaticMesh();
		const bool bPrivateStaticMesh = StaticMesh && !StaticMesh->HasAnyFlags(RF_Public);
		const bool bTransientStaticMesh = StaticMesh && StaticMesh->HasAnyFlags(RF_Transient);
		if (!StaticMesh || bPrivateStaticMesh || bTransientStaticMesh)
		{
			UE_LOG(LogHLODBuilder, Warning, TEXT("Instanced HLOD source component %s points to a %s static mesh, ignoring."), *SMC->GetPathName(), !StaticMesh ? TEXT("null") : bPrivateStaticMesh ? TEXT("private") : TEXT("transient"));
			continue;
		}

		TUniquePtr<FISMComponentDescriptor> ISMComponentDescriptor = ComponentClass->GetDefaultObject<UHLODInstSMComponentCustomPrim>()->AllocateISMComponentDescriptor();
		ISMComponentDescriptor->InitFrom(SMC, false);

		FCustomPrimInstanceBatch& InstanceBatch = InstancesData.FindOrAdd(ISMComponentDescriptor->GetTypeHash());
		if (!InstanceBatch.ISMComponentDescriptor.IsValid())
		{
			InstanceBatch.ISMComponentDescriptor = MoveTemp(ISMComponentDescriptor);
		}
		
		InstanceBatch.ISMComponentBatcher.Add(SMC);
	}

	// Create an ISMC for each SM asset we found
	TArray<UActorComponent*> HLODComponents;
	for (auto& Entry : InstancesData)
	{
		const FCustomPrimInstanceBatch& InstanceBatch = Entry.Value;

		UInstancedStaticMeshComponent* ISMComponent = InstanceBatch.ISMComponentDescriptor->CreateComponent(GetTransientPackage());
		InstanceBatch.ISMComponentBatcher.InitComponent(ISMComponent);
				
		HLODComponents.Add(ISMComponent);
	};

	return HLODComponents;
}
