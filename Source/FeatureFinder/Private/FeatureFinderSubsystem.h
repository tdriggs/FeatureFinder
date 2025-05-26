#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "FeatureFinderTypes.h"
#include "FeatureFinderSubsystem.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS()
class UFeatureFinderSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	static UFeatureFinderSubsystem* Get();
	static UFeatureFinderSubsystem& GetChecked();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const TArray<TSharedPtr<FFeature>>& GetFeatures() const { return Features; }
	TSharedPtr<FFeature> GetFeature(const UFeatureTypeConfig* FeatureType) const;
	
	void RecacheAllFeatures();
	void TryRecacheFeature(const UFeatureTypeConfig* FeatureType);

	TSharedPtr<FFeature> GetLastSelectedFeature() const;
	void SetLastSelectedFeature(const TSharedPtr<FFeature>& FeaturePtr) const;

	DECLARE_MULTICAST_DELEGATE(FOnFeaturesUpdated);
	FOnFeaturesUpdated OnFeaturesUpdated;
	
private:
	void UpdateFeatureInstances(const TSharedPtr<FFeature>& FeaturePtr);
	
	TArray<TSharedPtr<FFeature>> Features;

	UPROPERTY()
	TArray<TObjectPtr<UObject>> CachedReferences;
};
