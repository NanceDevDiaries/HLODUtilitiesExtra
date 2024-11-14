// Copyright 2024 NanceDevDiaries. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldPartition/HLOD/HLODModifier.h"
#include "WorldPartitionHLODModifierNoWPO.generated.h"

/**
 * Add this modifier to your HLODLayer where you'd like to disable World Position Offset.
 * Also functions to populate instanced foliage or static mesh's PerInstanceCustomData so the material can read it and
 * you don't have to do it manually
 */
UCLASS()
class HLODUTILITIESEXTRAEDITOR_API UWorldPartitionHLODModifierNoWPO : public UWorldPartitionHLODModifier
{
	GENERATED_BODY()

public:
	virtual bool CanModifyHLOD(TSubclassOf<UHLODBuilder> InHLODBuilderClass) const override;
	virtual void EndHLODBuild(TArray<UActorComponent*>& InOutComponents) override;
	virtual void BeginHLODBuild(const FHLODBuildContext& InHLODBuildContext) override;
};
