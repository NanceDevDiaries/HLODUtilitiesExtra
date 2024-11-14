// Copyright 2024 NanceDevDiaries. All Rights Reserved.

using UnrealBuildTool;

public class HLODUtilitiesExtraEditor : ModuleRules
{
    public HLODUtilitiesExtraEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
            }
        );
    }
}