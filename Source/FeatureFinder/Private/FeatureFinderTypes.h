#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "FeatureFinderTypes.generated.h"

class FFeatureAttributeConfigCustomization;
class UFeatureTypeConfig;
struct FEditableFeatureInstance;
struct FFeatureAttributeValue;
struct FFeatureInstance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM()
enum class EFeatureAttributeType
{
	Invalid UMETA(Hidden),
	Text,
	Folder,
	Asset,
	Property,
	Command,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfigInternal
{
	GENERATED_BODY()

	friend FFeatureAttributeConfigCustomization;

	virtual ~FFeatureAttributeConfigInternal() {}

	// The name of this attribute. Must be unique for this feature.
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayPriority = -1))
	FName Name = NAME_None;

	// If false, this attribute will not be displayed in the feature finder window.
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayPriority = -1))
	bool bIsVisible = true;

	// If true, this feature will be generated based on other feature attributes. Use {AttributeName} to replace
	// parts of the attribute with other attribute values.
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayPriority = -1))
	bool bIsGenerated = false;

	const EFeatureAttributeType& GetType() const { return Type; }

	FFeatureAttributeValue MakeValueStruct() const;
	virtual void InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const {}

	FFeatureAttributeValue GenerateValueStruct(const FFeatureInstance& Instance) const;
	virtual void GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const {}

protected:
	UPROPERTY()
	EFeatureAttributeType Type = EFeatureAttributeType::Invalid;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfig
{
	GENERATED_BODY()

	TInstancedStruct<FFeatureAttributeConfigInternal>* operator->() { return &Config; }
	const TInstancedStruct<FFeatureAttributeConfigInternal>* operator->() const { return &Config; }

	UPROPERTY(EditDefaultsOnly, Meta = (ExcludeBaseStruct))
	TInstancedStruct<FFeatureAttributeConfigInternal> Config;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeValueInternal
{
	GENERATED_BODY()

	virtual ~FFeatureAttributeValueInternal() {}

	virtual void InitializeFromConfig(const FFeatureAttributeConfigInternal& Config);
	virtual void CopyFrom(const FFeatureAttributeValueInternal* Other) {}
	virtual void LoadAndCacheReferences(TArray<TObjectPtr<UObject>>& OutReferences) const {}
	virtual bool MatchesFilterString(const FString& FilterString) const { return false; }
	virtual FString ToString() const { return FString(); }

	const FName& GetName() const { return Name; }
	const EFeatureAttributeType& GetType() const { return Type; }

	bool Matches(const FFeatureAttributeValueInternal& Other) const { return Name == Other.Name && Type == Other.Type; }

protected:
	UPROPERTY()
	FName Name = NAME_None;

	UPROPERTY()
	EFeatureAttributeType Type = EFeatureAttributeType::Invalid;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeValue
{
	GENERATED_BODY()

	TInstancedStruct<FFeatureAttributeValueInternal>* operator->() { return &Value; }
	const TInstancedStruct<FFeatureAttributeValueInternal>* operator->() const { return &Value; }

	UPROPERTY(EditDefaultsOnly, Meta = (ExcludeBaseStruct))
	TInstancedStruct<FFeatureAttributeValueInternal> Value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FFeatureInstance : TSharedFromThis<FFeatureInstance>
{
	TArray<FFeatureAttributeValue> Attributes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FFeature : TSharedFromThis<FFeature>
{
	TWeakObjectPtr<const UFeatureTypeConfig> Type;
	TArray<TSharedPtr<FFeatureInstance>> Instances;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FEditableFeatureInstance
{
	GENERATED_BODY()

	FFeatureInstance ToFeatureInstance() const;
	void FromFeatureInstance(const FFeatureInstance& FeatureInstance);

	void RefreshAttributes(const UFeatureTypeConfig* FeatureType);

	UPROPERTY(EditDefaultsOnly)
	TArray<FFeatureAttributeValue> Attributes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS()
class UFeatureTypeConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// The display name of this feature type.
	UPROPERTY(EditDefaultsOnly)
	FString DisplayName = FString();

	// A list of attribute configs that define this feature type.
	UPROPERTY(EditDefaultsOnly)
	TArray<FFeatureAttributeConfig> Attributes;

	// A data table that defines features for this feature type.
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> FeaturesTable;

	// A list of manually defined features for this feature type.
	UPROPERTY(EditDefaultsOnly)
	TArray<FEditableFeatureInstance> Features;

	const FFeatureAttributeConfig& GetAttribute(const FName& AttributeName) const;
	TArray<FName> GetGeneratedAttributeNames() const;
	FFeatureInstance MakeFeatureInstance() const;

private:
	virtual bool IsEditorOnly() const override { return true; }

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	#endif // WITH_EDITOR
};
