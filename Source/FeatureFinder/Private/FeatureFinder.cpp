#include "CoreMinimal.h"
#include "FeatureFinderTypeCustomizations.h"
#include "FeatureFinderTypes.h"
#include "Framework/Docking/LayoutExtender.h"
#include "LevelEditor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "SFeatureFinderWidget.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName FeatureFinderTabName = TEXT("FeatureFinderTab");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
class FFeatureFinderModule : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	template <typename PropertyType, typename CustomizationType>
	void RegisterCustomPropertyTypeLayout(FPropertyEditorModule& PropertyModule)
	{
		const FName PropertyTypeName = PropertyType::StaticStruct()->GetFName();
		PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, FOnGetPropertyTypeCustomizationInstance::CreateStatic(&CustomizationType::MakeInstance));
	}

	void RegisterFeatureFinderTab(TSharedPtr<FTabManager> TabManager);
	void RegisterFeatureFinderLayout(FLayoutExtender& LayoutExtender);

	TSharedRef<SDockTab> SpawnFeatureFinderTab(const FSpawnTabArgs& Args) const;
};

IMPLEMENT_GAME_MODULE(FFeatureFinderModule, FeatureFinder);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureFinderModule::StartupModule()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	if (!IsRunningCommandlet())
	{
		LevelEditorModule.OnRegisterTabs().AddRaw(this, &FFeatureFinderModule::RegisterFeatureFinderTab);
		LevelEditorModule.OnRegisterLayoutExtensions().AddRaw(this, &FFeatureFinderModule::RegisterFeatureFinderLayout);
	}

	RegisterCustomPropertyTypeLayout<FFeatureAttributeConfig, FFeatureAttributeConfigCustomization>(PropertyModule);
	RegisterCustomPropertyTypeLayout<FFeatureAttributeValue, FFeatureAttributeValueCustomization>(PropertyModule);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureFinderModule::ShutdownModule()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!IsRunningCommandlet())
	{
		LevelEditorModule.OnRegisterTabs().RemoveAll(this);
		LevelEditorModule.OnRegisterLayoutExtensions().RemoveAll(this);

		if (TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager())
		{
			TabManager->UnregisterTabSpawner(FeatureFinderTabName);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureFinderModule::RegisterFeatureFinderTab(TSharedPtr<FTabManager> TabManager)
{
	TabManager->RegisterTabSpawner(FeatureFinderTabName,
		          FOnSpawnTab::CreateRaw(this, &FFeatureFinderModule::SpawnFeatureFinderTab))
	          .SetDisplayName(FText::FromString(TEXT("Feature Finder")))
	          .SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
	          .SetIcon(FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Symbols.SearchGlass"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureFinderModule::RegisterFeatureFinderLayout(FLayoutExtender& LayoutExtender)
{
	LayoutExtender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::After,
		FTabManager::FTab(FeatureFinderTabName, ETabState::OpenedTab));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SDockTab> FFeatureFinderModule::SpawnFeatureFinderTab(const FSpawnTabArgs& Args) const
{
	return SNew(SDockTab)
		.TabRole(PanelTab)
		[
			SNew(SFeatureFinderWidget)
		];
}
