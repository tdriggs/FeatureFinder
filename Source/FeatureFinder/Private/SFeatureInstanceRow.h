#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

class UFeatureTypeConfig;
struct FFeatureAttributeValue_Text;
struct FFeatureAttributeValue_Folder;
struct FFeatureAttributeValue_Asset;
struct FFeatureAttributeValue_Property;
struct FFeatureAttributeValue_Command;
struct FFeatureInstance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SFeatureInstanceRow : public SMultiColumnTableRow<TSharedPtr<FFeatureInstance>>
{
public:
	SLATE_BEGIN_ARGS(SFeatureInstanceRow) {}
		SLATE_ARGUMENT(TSharedPtr<FFeatureInstance>, FeatureInstance)
		SLATE_ARGUMENT(TWeakObjectPtr<const UFeatureTypeConfig>, FeatureType)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable);
	
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	TSharedRef<SWidget> GenerateTextValueWidget(const FFeatureAttributeValue_Text& Attribute) const;
	TSharedRef<SWidget> GenerateFolderValueWidget(const FFeatureAttributeValue_Folder& Attribute) const;
	TSharedRef<SWidget> GenerateAssetValueWidget(const FFeatureAttributeValue_Asset& Attribute) const;
	TSharedRef<SWidget> GeneratePropertyValueWidget(const FFeatureAttributeValue_Property& Attribute) const;
	TSharedRef<SWidget> GenerateCommandValueWidget(const FFeatureAttributeValue_Command& Attribute) const;

	bool CommandValue_IsEnabled(const FFeatureAttributeValue_Command Attribute) const;
	
	FReply FolderValue_OnBrowseClicked(const FFeatureAttributeValue_Folder Attribute) const;
	FReply AssetValue_OnBrowseClicked(const FFeatureAttributeValue_Asset Attribute) const;
	FReply AssetValue_OnEditClicked(const FFeatureAttributeValue_Asset Attribute) const;
	FReply CommandValue_OnCommandClicked(const FFeatureAttributeValue_Command Attribute) const;
	FReply CommandValue_OnCopyClicked(const FFeatureAttributeValue_Command Attribute) const;
	
	TSharedPtr<FFeatureInstance> FeatureInstancePtr;
	TWeakObjectPtr<const UFeatureTypeConfig> FeatureTypePtr;
};
