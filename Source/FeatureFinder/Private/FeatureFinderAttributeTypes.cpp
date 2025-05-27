#include "FeatureFinderAttributeTypes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Text::InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const
{
	ValueStruct->InitializeAs<FFeatureAttributeValue_Text>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Text::GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const
{
	ValueStruct->GetMutable<FFeatureAttributeValue_Text>().Text = FText::Format(FTextFormat::FromString(TextFormat), ValueArgs).ToString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Text::CopyFrom(const FFeatureAttributeValueInternal* Other)
{
	if (Other && Other->GetType() == EFeatureAttributeType::Text)
	{
		Text = static_cast<const FFeatureAttributeValue_Text*>(Other)->Text;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FFeatureAttributeValue_Text::MatchesFilterString(const FString& FilterString) const
{
	return Text.Contains(FilterString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FFeatureAttributeValue_Text::ToString() const
{
	return Text;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Folder::InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const
{
	ValueStruct->InitializeAs<FFeatureAttributeValue_Folder>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Folder::GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const
{
	ValueStruct->GetMutable<FFeatureAttributeValue_Folder>().Folder = FText::Format(FTextFormat::FromString(FolderFormat), ValueArgs).ToString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Folder::CopyFrom(const FFeatureAttributeValueInternal* Other)
{
	if (Other && Other->GetType() == EFeatureAttributeType::Folder)
	{
		Folder = static_cast<const FFeatureAttributeValue_Folder*>(Other)->Folder;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FFeatureAttributeValue_Folder::MatchesFilterString(const FString& FilterString) const
{
	return Folder.Contains(FilterString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FFeatureAttributeValue_Folder::ToString() const
{
	return Folder;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Asset::InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const
{
	ValueStruct->InitializeAs<FFeatureAttributeValue_Asset>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Asset::GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const
{
	const FString AssetPath = FText::Format(FTextFormat::FromString(AssetPathFormat), ValueArgs).ToString();
	ValueStruct->GetMutable<FFeatureAttributeValue_Asset>().Asset = FSoftObjectPath(AssetPath).TryLoad();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Asset::CopyFrom(const FFeatureAttributeValueInternal* Other)
{
	if (Other && Other->GetType() == EFeatureAttributeType::Asset)
	{
		Asset = static_cast<const FFeatureAttributeValue_Asset*>(Other)->Asset;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Asset::LoadAndCacheReferences(TArray<TObjectPtr<UObject>>& OutReferences) const
{
	if (UObject* Object = Asset.LoadSynchronous())
	{
		OutReferences.Add(Object);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FFeatureAttributeValue_Asset::MatchesFilterString(const FString& FilterString) const
{
	return GetNameSafe(Asset.Get()).Contains(FilterString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FFeatureAttributeValue_Asset::ToString() const
{
	return GetNameSafe(Asset.Get());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Property::InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const
{
	ValueStruct->InitializeAs<FFeatureAttributeValue_Property>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Property::GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const
{
	const FString AssetPath = FText::Format(FTextFormat::FromString(AssetPathFormat), ValueArgs).ToString();
	ValueStruct->GetMutable<FFeatureAttributeValue_Property>().Asset = FSoftObjectPath(AssetPath).TryLoad();
	ValueStruct->GetMutable<FFeatureAttributeValue_Property>().PropertyName = FText::Format(FTextFormat::FromString(PropertyNameFormat), ValueArgs).ToString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Property::CopyFrom(const FFeatureAttributeValueInternal* Other)
{
	if (Other && Other->GetType() == EFeatureAttributeType::Property)
	{
		Asset = static_cast<const FFeatureAttributeValue_Property*>(Other)->Asset;
		PropertyName = static_cast<const FFeatureAttributeValue_Property*>(Other)->PropertyName;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Property::LoadAndCacheReferences(TArray<TObjectPtr<UObject>>& OutReferences) const
{
	if (UObject* Object = Asset.LoadSynchronous())
	{
		OutReferences.Add(Object);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FFeatureAttributeValue_Property::MatchesFilterString(const FString& FilterString) const
{
	const FString PropertyString = ToString();
	return PropertyString.Contains(FilterString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FFeatureAttributeValue_Property::ToString() const
{
	FProperty* AttributeProperty = nullptr;
	if (Asset)
	{
		for (TFieldIterator<FProperty> Iterator(Asset->GetClass()); Iterator; ++Iterator)
		{
			FProperty* Property = *Iterator;
			if (Property && Property->GetName() == PropertyName)
			{
				AttributeProperty = Property;
				break;
			}
		}
	}

	if (!AttributeProperty)
	{
		return TEXT("Invalid");
	}
	
	FString PropertyValue = TEXT("Unsupported");
	if (AttributeProperty->IsA<FTextProperty>())
	{
		const FTextProperty* TextProperty = CastFieldChecked<FTextProperty>(AttributeProperty);
		const FText Text = TextProperty->GetPropertyValue_InContainer(Asset.Get());
		PropertyValue = Text.ToString();
	}
	else if (AttributeProperty->IsA<FStrProperty>())
	{
		const FStrProperty* StrProperty = CastFieldChecked<FStrProperty>(AttributeProperty);
		PropertyValue = StrProperty->GetPropertyValue_InContainer(Asset.Get());
	}
	else if (AttributeProperty->IsA<FObjectProperty>())
	{
		const FObjectProperty* ObjectProperty = CastFieldChecked<FObjectProperty>(AttributeProperty);
		const UObject* Object = ObjectProperty->GetPropertyValue_InContainer(Asset.Get());
		PropertyValue = GetNameSafe(Object);
	}
	else if (AttributeProperty->IsA<FNumericProperty>())
	{
		const FNumericProperty* NumericProperty = CastFieldChecked<FNumericProperty>(AttributeProperty);
		PropertyValue = NumericProperty->GetNumericPropertyValueToString_InContainer(Asset.Get());
	}
	else if (AttributeProperty->IsA<FNameProperty>())
	{
		const FNameProperty* NameProperty = CastFieldChecked<FNameProperty>(AttributeProperty);
		const FName NameValue = NameProperty->GetPropertyValue_InContainer(Asset.Get());
		PropertyValue = NameValue.ToString();
	}
	else if (AttributeProperty->IsA<FBoolProperty>())
	{
		const FBoolProperty* BoolProperty = CastFieldChecked<FBoolProperty>(AttributeProperty);
		const bool Bool = BoolProperty->GetPropertyValue_InContainer(Asset.Get());
		PropertyValue = Bool ? TEXT("True") : TEXT("False");
	}

	return PropertyValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Command::InitializeValueStructType(FFeatureAttributeValue& ValueStruct) const
{
	ValueStruct->InitializeAs<FFeatureAttributeValue_Command>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeConfig_Command::GenerateValueFromArgs(FFeatureAttributeValue& ValueStruct, const FFormatNamedArguments& ValueArgs) const
{
	ValueStruct->GetMutable<FFeatureAttributeValue_Command>().Command = FText::Format(FTextFormat::FromString(CommandFormat), ValueArgs).ToString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValue_Command::CopyFrom(const FFeatureAttributeValueInternal* Other)
{
	if (Other && Other->GetType() == EFeatureAttributeType::Command)
	{
		Command = static_cast<const FFeatureAttributeValue_Command*>(Other)->Command;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FFeatureAttributeValue_Command::MatchesFilterString(const FString& FilterString) const
{
	return Command.Contains(FilterString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString FFeatureAttributeValue_Command::ToString() const
{
	return Command;
}
