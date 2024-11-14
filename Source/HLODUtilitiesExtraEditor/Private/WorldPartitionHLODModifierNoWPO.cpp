// Copyright 2024 NanceDevDiaries. All Rights Reserved.

#include "WorldPartitionHLODModifierNoWPO.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "WorldPartition/HLOD/HLODBuilder.h"

bool UWorldPartitionHLODModifierNoWPO::CanModifyHLOD(TSubclassOf<UHLODBuilder> InHLODBuilderClass) const
{
	return true;
}

void UWorldPartitionHLODModifierNoWPO::EndHLODBuild(TArray<UActorComponent*>& InOutComponents)
{
	for (const auto& OutComponent : InOutComponents)
	{
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(OutComponent))
		{
			StaticMeshComponent->SetEvaluateWorldPositionOffset(false);
		}
	}
}

void UWorldPartitionHLODModifierNoWPO::BeginHLODBuild(const FHLODBuildContext& InHLODBuildContext)
{
	// Support for instanced meshes for custom data to be sent to the PerInstanceSMCustomData for the HLOD
	// PerInstanceSMCustomData to work.
	// Case : foliage ISM that had custom data but no PerInstanceSMCustomData, this should support making it by hand too.

	for (const auto SourceComponent : InHLODBuildContext.SourceComponents)
	{
		if (UInstancedStaticMeshComponent* ISMComp = Cast<UInstancedStaticMeshComponent>(SourceComponent))
		{
			TArray<float> CustomPrimitiveDataToMultiply = ISMComp->GetDefaultCustomPrimitiveData().Data;
			int32 CustomPrimitiveDataNum = CustomPrimitiveDataToMultiply.Num();
			if (CustomPrimitiveDataNum > 0 && ISMComp->PerInstanceSMCustomData.Num() == 0)
			{
				// means we want all its instances to have this specific custom primitive data
				ISMComp->SetNumCustomDataFloats(CustomPrimitiveDataNum);
				for (int32 InstanceIdx = 0; InstanceIdx < ISMComp->GetInstanceCount(); InstanceIdx++)
				{
					ISMComp->SetCustomData(InstanceIdx, CustomPrimitiveDataToMultiply, true);
				}
			}
		}
	}
}
