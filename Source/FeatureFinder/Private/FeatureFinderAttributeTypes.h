#pragma once

#include "CoreMinimal.h"
#include "FeatureFinderTypes.h"
#include "FeatureFinderAttributeTypes.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ========== TEXT ==========
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfig_Text : public FFeatureAttributeConfigInternal
{
	GENERATED_BODY()

public:
	FFeatureAttributeConfig_Text() { Type = EFeatureAttributeType::Text; }
	virtual void InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const override;
	virtual void GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const override;
	
	// The format to use for the text if this attribute is generated.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bIsGenerated", EditConditionHides))
	FString TextFormat = FString();
};

USTRUCT(BlueprintType)
struct FFeatureAttributeValue_Text : public FFeatureAttributeValueInternal
{
	GENERATED_BODY()
	
public:
	FFeatureAttributeValue_Text() { Type = EFeatureAttributeType::Text; }

	virtual void CopyFrom(const FFeatureAttributeValueInternal* Other) override;
	virtual bool MatchesFilterString(const FString& FilterString) const override;
	virtual FString ToString() const override;
	
	UPROPERTY(EditDefaultsOnly)
	FString Text = FString();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ========== FOLDER ==========
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfig_Folder : public FFeatureAttributeConfigInternal
{
	GENERATED_BODY()

public:
	FFeatureAttributeConfig_Folder() { Type = EFeatureAttributeType::Folder; }
	virtual void InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const override;
	virtual void GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const override;
	
	// The format to use for the folder if this attribute is generated.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bIsGenerated", EditConditionHides))
	FString FolderFormat = FString();
};

USTRUCT(BlueprintType)
struct FFeatureAttributeValue_Folder : public FFeatureAttributeValueInternal
{
	GENERATED_BODY()
	
public:
	FFeatureAttributeValue_Folder() { Type = EFeatureAttributeType::Folder; }

	virtual void CopyFrom(const FFeatureAttributeValueInternal* Other) override;
	virtual bool MatchesFilterString(const FString& FilterString) const override;
	virtual FString ToString() const override;
	
	UPROPERTY(EditDefaultsOnly)
	FString Folder = FString();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ========== ASSET ==========
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfig_Asset : public FFeatureAttributeConfigInternal
{
	GENERATED_BODY()

public:
	FFeatureAttributeConfig_Asset() { Type = EFeatureAttributeType::Asset; }
	virtual void InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const override;
	virtual void GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const override;
	
	// The format to use for the asset's path if this attribute is generated.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bIsGenerated", EditConditionHides))
	FString AssetPathFormat = FString();
};

USTRUCT(BlueprintType)
struct FFeatureAttributeValue_Asset : public FFeatureAttributeValueInternal
{
	GENERATED_BODY()
	
public:
	FFeatureAttributeValue_Asset() { Type = EFeatureAttributeType::Asset; }

	virtual void CopyFrom(const FFeatureAttributeValueInternal* Other) override;
	virtual void LoadAndCacheReferences(TArray<TObjectPtr<UObject>>& OutReferences) const override;
	virtual bool MatchesFilterString(const FString& FilterString) const override;
	virtual FString ToString() const override;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UObject> Asset = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ========== PROPERTY ==========
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfig_Property : public FFeatureAttributeConfigInternal
{
	GENERATED_BODY()

public:
	FFeatureAttributeConfig_Property() { Type = EFeatureAttributeType::Property; }
	virtual void InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const override;
	virtual void GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const override;
	
	// The format to use for the asset's path if this attribute is generated.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bIsGenerated", EditConditionHides))
	FString AssetPathFormat = FString();
	
	// The format to use for the property's name if this attribute is generated.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bIsGenerated", EditConditionHides))
	FString PropertyNameFormat = FString();
};

USTRUCT(BlueprintType)
struct FFeatureAttributeValue_Property : public FFeatureAttributeValueInternal
{
	GENERATED_BODY()
	
public:
	FFeatureAttributeValue_Property() { Type = EFeatureAttributeType::Property; }

	virtual void CopyFrom(const FFeatureAttributeValueInternal* Other) override;
	virtual void LoadAndCacheReferences(TArray<TObjectPtr<UObject>>& OutReferences) const override;
	virtual bool MatchesFilterString(const FString& FilterString) const override;
	virtual FString ToString() const override;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UObject> Asset = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	FString PropertyName = FString();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ========== COMMAND ==========
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FFeatureAttributeConfig_Command : public FFeatureAttributeConfigInternal
{
	GENERATED_BODY()

public:
	FFeatureAttributeConfig_Command() { Type = EFeatureAttributeType::Command; }
	virtual void InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const override;
	virtual void GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const override;

	// The format to use for the command if this attribute is generated.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bIsGenerated", EditConditionHides))
	FString CommandFormat = FString();
	
	// If true, OverrideDisplayCommand will be shown on the command button in the FeatureFinder window instead of the command.
	UPROPERTY(EditDefaultsOnly)
	bool bUseOverrideDisplayCommand = false;

	// The display command to show on the command button in the FeatureFinder window instead of the command.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bUseOverrideDisplayCommand", EditConditionHides))
	FString OverrideDisplayCommand = FString();
	
	// If true, this command line will only be run if there is an active PIE context.
	UPROPERTY(EditDefaultsOnly)
	bool bOnlyRunInPIE = false;

	// If true, this command will be executed on the first PIE instance in standalone mode.
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayName = "Only Run On First PIE Standalone Instance"))
	bool bOnlyRunOnFirstPIEStandalone = false;

	// If true, this command will be executed on PIE servers in multiplayer mode.
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayName = "Run On PIE Server Instances"))
	bool bRunOnPIEServers = false;

	// If true, this command will be executed on PIE clients in multiplayer mode.
	UPROPERTY(EditDefaultsOnly, Meta = (DisplayName = "Run On PIE Client Instances"))
	bool bRunOnPIEClients = false;

	// If true, this command will be executed on the first PIE client in multiplayer mode.
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bRunOnPIEClients", EditConditionHides, DisplayName = "Only Run On First PIE Client Instance"))
	bool bOnlyRunOnFirstPIEClient = false;
};

USTRUCT(BlueprintType)
struct FFeatureAttributeValue_Command : public FFeatureAttributeValueInternal
{
	GENERATED_BODY()
	
public:
	FFeatureAttributeValue_Command() { Type = EFeatureAttributeType::Command; }

	virtual void CopyFrom(const FFeatureAttributeValueInternal* Other) override;
	virtual bool MatchesFilterString(const FString& FilterString) const override;
	virtual FString ToString() const override;
	
	UPROPERTY(EditDefaultsOnly)
	FString Command = FString();
};
