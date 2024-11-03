// Copyright 2024 NanceDevDiaries. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HLODBuilderInstancing.h"
#include "HLODBuilderInstancingCustomPrim.generated.h"

/**
 * For extra settings that you'd want to apply to the HLOD components
 */
UCLASS(Blueprintable, Config = Engine, PerObjectConfig)
class HLODUTILITIESEXTRAEDITOR_API UHLODBuilderInstancingCustomSettings : public UHLODBuilderInstancingSettings
{
	GENERATED_UCLASS_BODY()

	virtual uint32 GetCRC() const override;
	
	/**
	 * If enabled, the components created for the HLODs will not use World Position Offset.
	 * Some foliage was was behaving oddly when batched together in HLODs, this option is there if you have the same issue
	 * Looking like floating specters on a world scale
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = Rendering)
	bool bDisallowWorldPositionOffset;
};

/**
 * Build a AWorldPartitionHLOD whose components are ISMC.
 * Allows batching with added functionality for custom primitive data and how that batches
 */
UCLASS(HideDropdown)
class HLODUTILITIESEXTRAEDITOR_API UHLODBuilderInstancingCustomPrim : public UHLODBuilderInstancing
{
	GENERATED_UCLASS_BODY()

public:
	virtual TSubclassOf<UHLODBuilderSettings> GetSettingsClass() const override;
	virtual TArray<UActorComponent*> Build(const FHLODBuildContext& InHLODBuildContext,
		const TArray<UActorComponent*>& InSourceComponents) const override;

protected:
	/* Implemented our own batching because there's no nice way to modify/append the existing batching logic to
	 * also consider customPrimitiveData. */
	static TArray<UActorComponent*> BatchInstances(const TArray<UActorComponent*>& InSourceComponents);
};

