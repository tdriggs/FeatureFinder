using UnrealBuildTool;

public class FeatureFinder : ModuleRules
{
	public FeatureFinder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]{});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"Slate",
			"SlateCore",
			"WorkspaceMenuStructure",
			"EditorSubsystem",
			"DeveloperSettings",
			"InputCore",
			"Blutility",
			"ApplicationCore",
			"StructUtilsEditor",
		});
	}
}
