#include "FeatureFinderTypes.h"
#include "FeatureFinderSubsystem.h"
#include "Misc/DataValidation.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FFeatureAttributeValue FFeatureAttributeConfigInternal::MakeValueStruct() const
{
	FFeatureAttributeValue ValueStruct;
	InitializeValueStructType(ValueStruct);
	if (ValueStruct->IsValid())
	{
		ValueStruct->GetMutable().InitializeFromConfig(*this);
	}
	
	return ValueStruct;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FFeatureAttributeValue FFeatureAttributeConfigInternal::GenerateValueStruct(const FFeatureInstance& Instance) const
{
	FFeatureAttributeValue ValueStruct = MakeValueStruct();
	
	FFormatNamedArguments FormatNamedArguments;
	for (const FFeatureAttributeValue& AttributeValue : Instance.Attributes)
	{
		if (AttributeValue->IsValid())
		{
			FormatNamedArguments.Add(AttributeValue->Get().GetName().ToString(), FFormatArgumentValue(FText::FromString(AttributeValue->Get().ToString())));	
		}
	}
	
	GenerateValueFromArgs(ValueStruct, FormatNamedArguments);
	return ValueStruct;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FFeatureAttributeValueInternal::InitializeFromConfig(const FFeatureAttributeConfigInternal& Config)
{
	Name = Config.Name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FFeatureInstance FEditableFeatureInstance::ToFeatureInstance() const
{
	FFeatureInstance FeatureInstance;
	FeatureInstance.Attributes = Attributes;
	return FeatureInstance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FEditableFeatureInstance::FromFeatureInstance(const FFeatureInstance& FeatureInstance)
{
	Attributes = FeatureInstance.Attributes;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FEditableFeatureInstance::RefreshAttributes(const UFeatureTypeConfig* FeatureType)
{
	if (!FeatureType)
	{
		Attributes.Reset();
		return;
	}
	
	const TArray<FFeatureAttributeValue> OldAttributes = Attributes;
	FromFeatureInstance(FeatureType->MakeFeatureInstance());

	TArray<int32> MatchedIndices;
	for (const FFeatureAttributeValue& Value : OldAttributes)
	{
		if (!Value->IsValid())
		{
			continue;
		}
			
		for (int32 ValueIndex = 0; ValueIndex < Attributes.Num(); ++ValueIndex)
		{
			if (Value->Get().Matches(Attributes[ValueIndex]->Get()))
			{
				Attributes[ValueIndex] = Value;
				MatchedIndices.Add(ValueIndex);
				break;
			}
		}
	}

	for (int32 ValueIndex = 0; ValueIndex < Attributes.Num(); ++ValueIndex)
	{
		if (MatchedIndices.Contains(ValueIndex) || !OldAttributes.IsValidIndex(ValueIndex))
		{
			continue;
		}

		const FFeatureAttributeValueInternal* OldValue = OldAttributes[ValueIndex]->GetPtr();
		FFeatureAttributeValueInternal* NewValue = Attributes[ValueIndex]->GetMutablePtr();

		if (OldValue && NewValue && OldValue->GetType() == NewValue->GetType())
		{
			NewValue->CopyFrom(OldValue);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const FFeatureAttributeConfig& UFeatureTypeConfig::GetAttribute(const FName& AttributeName) const
{
	for (const FFeatureAttributeConfig& Attribute : Attributes)
	{
		if (Attribute->IsValid() && Attribute->Get().Name == AttributeName)
		{
			return Attribute;
		}
	}

	static const FFeatureAttributeConfig InvalidAttribute;
	return InvalidAttribute;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TArray<FName> UFeatureTypeConfig::GetGeneratedAttributeNames() const
{
	TArray<FName> Names;
	for (const FFeatureAttributeConfig& Config : Attributes)
	{
		if (Config->IsValid() && Config->Get().bIsGenerated)
		{
			Names.Add(Config->Get().Name);
		}
	}

	return Names;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FFeatureInstance UFeatureTypeConfig::MakeFeatureInstance() const
{
	FFeatureInstance Instance;
	Instance.Attributes.Reserve(Attributes.Num());
	
	for (const FFeatureAttributeConfig& Config : Attributes)
	{
		if (Config->IsValid() && !Config->Get().bIsGenerated)
		{
			Instance.Attributes.Add(Config->Get().MakeValueStruct());
		}
	}

	return Instance;
}

#if WITH_EDITOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureTypeConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (FEditableFeatureInstance& Feature : Features)
	{
		Feature.RefreshAttributes(this);
	}

	UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
	FeatureFinderSubsystem.TryRecacheFeature(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EDataValidationResult UFeatureTypeConfig::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	TSet<FName> FoundNames;
	for (const FFeatureAttributeConfig& Config : Attributes)
	{
		if (!Config->IsValid())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Attribute Config has invalid type. Select a valid type."))));
			Result = EDataValidationResult::Invalid;
			continue;
		}
		
		if (Config->Get().Name.IsNone())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Attribute Config [%s] has an invalid name. Input a valid name."), *Config->Get().Name.ToString())));
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (FoundNames.Contains(Config->Get().Name))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Attribute Config [%s] has duplicate name. Each attribute must have a unique name."), *Config->Get().Name.ToString())));
			Result = EDataValidationResult::Invalid;
		}

		FoundNames.Add(Config->Get().Name);
	}
	
	return Result;
}
#endif // WITH_EDITOR
