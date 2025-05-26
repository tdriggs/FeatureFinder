#include "FeatureFinderTypeCustomizations.h"
#include "DetailWidgetRow.h"
#include "FeatureFinderTypes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename EnumType>
FString GetEnumName(EnumType Value)
{
	if (const UEnum* Enum = StaticEnum<std::decay_t<EnumType>>())
	{
		const int64 Index = Enum->GetIndexByValue(static_cast<int64>(Value));
		if (Index != INDEX_NONE)
		{
			return Enum->GetNameStringByIndex(Index);
		}
	}

	return TEXT("Invalid");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfigCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> PropertyHandlePtr = PropertyHandle;
	TSharedPtr<IPropertyHandle> ConfigPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFeatureAttributeConfig, Config));
	FInstancedStructDetails::CustomizeHeader(ConfigPropertyHandle.ToSharedRef(), HeaderRow, CustomizationUtils);
	
	HeaderRow
	.NameContent()
	[
		SNew(STextBlock)
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
		.Text_Lambda([PropertyHandlePtr]()
		{
			if (PropertyHandlePtr.IsValid())
			{
				TArray<void*> RawPropertyData;
				PropertyHandlePtr->AccessRawData(RawPropertyData);
				FFeatureAttributeConfig* Config = static_cast<FFeatureAttributeConfig*>(RawPropertyData[0]);
				if (Config && (*Config)->IsValid())
				{
					return FText::FromName((*Config)->Get().Name);
				}
			}
			
			return FText::FromString(TEXT("Invalid"));
		})
	];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfigCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> ConfigPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFeatureAttributeConfig, Config));
	FInstancedStructDetails::CustomizeChildren(ConfigPropertyHandle.ToSharedRef(), ChildBuilder, CustomizationUtils);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValueCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> PropertyHandlePtr = PropertyHandle;
	TSharedPtr<IPropertyHandle> ValuePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFeatureAttributeValue, Value));
	FInstancedStructDetails::CustomizeHeader(ValuePropertyHandle.ToSharedRef(), HeaderRow, CustomizationUtils);
	
	HeaderRow
	.NameContent()
	[
		SNew(STextBlock)
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
		.Text_Lambda([PropertyHandlePtr]()
		{
			if (PropertyHandlePtr.IsValid())
			{
				TArray<void*> RawPropertyData;
				PropertyHandlePtr->AccessRawData(RawPropertyData);
				FFeatureAttributeValue* Value = static_cast<FFeatureAttributeValue*>(RawPropertyData[0]);
				if (Value && (*Value)->IsValid())
				{
					return FText::FromString(FString::Printf(TEXT("%s (%s)"), *(*Value)->Get().GetName().ToString(), *GetEnumName((*Value)->Get().GetType())));
				}
			}
			
			return FText::FromString(TEXT("Invalid"));
		})
	]
	.ValueContent()
	[
		SNew(SBox)
		.Visibility(EVisibility::Collapsed)
	];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValueCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> ValuePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFeatureAttributeValue, Value));
	FInstancedStructDetails::CustomizeChildren(ValuePropertyHandle.ToSharedRef(), ChildBuilder, CustomizationUtils);
}
