// Copyright 2024 NanceDevDiaries. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldPartition/HLOD/HLODInstancedStaticMeshComponent.h"
#include "HLODInstSMComponentCustomPrim.generated.h"

/**
 * 
 */
UCLASS(Hidden, NotPlaceable)
class HLODUTILITIESEXTRA_API UHLODInstSMComponentCustomPrim : public UHLODInstancedStaticMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	virtual TUniquePtr<FISMComponentDescriptor> AllocateISMComponentDescriptor() const override;
#endif
};

// ISM descriptor class based on FISMComponentDescriptor
USTRUCT()
struct FHLODISMCustomPrimComponentDescriptor : public FHLODISMComponentDescriptor
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual uint32 ComputeHash() const override;

	FHLODISMCustomPrimComponentDescriptor();
	virtual void InitComponent(UInstancedStaticMeshComponent* ISMComponent) const override;
	virtual void InitFrom(const UStaticMeshComponent* Template, bool bInitBodyInstance) override;
#endif

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Component Settings", meta = (DisplayAfter = "ComponentClass"))
	FCustomPrimitiveData DefaultCustomPrimData;
#endif
};