#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FeatureFinderTypes.h"
#include "FeatureFinderSettings.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(DefaultConfig, Config = "Editor", Meta = (DisplayName = "Feature Finder", Tooltip = "Settings for the Feature Finder plugin."))
class UFeatureFinderSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// The list of feature types to display in the Feature Finder window.
	UPROPERTY(Config, EditAnywhere, Category = "Features")
	TArray<TSoftObjectPtr<const UFeatureTypeConfig>> FeatureTypes;

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Config = EditorPerProjectUserSettings)
class UFeatureFinderUserSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	TSoftObjectPtr<const UFeatureTypeConfig> LastSelectedFeatureType;
};
