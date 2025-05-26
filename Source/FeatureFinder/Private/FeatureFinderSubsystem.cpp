#include "FeatureFinderSubsystem.h"
#include "FeatureFinderSettings.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UFeatureFinderSubsystem* UFeatureFinderSubsystem::Get()
{
	return GEditor ? GEditor->GetEditorSubsystem<UFeatureFinderSubsystem>() : nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UFeatureFinderSubsystem& UFeatureFinderSubsystem::GetChecked()
{
	UFeatureFinderSubsystem* Instance = GEditor->GetEditorSubsystem<UFeatureFinderSubsystem>();
	check(Instance);
	return *Instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureFinderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RecacheAllFeatures();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<FFeature> UFeatureFinderSubsystem::GetFeature(const UFeatureTypeConfig* FeatureType) const
{
	for (const TSharedPtr<FFeature>& FeaturePtr : Features)
	{
		const FFeature* Feature = FeaturePtr.Get();
		const UFeatureTypeConfig* Type = Feature ? Feature->Type.Get() : nullptr;
		if (Type && Type == FeatureType)
		{
			return FeaturePtr;
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureFinderSubsystem::RecacheAllFeatures()
{
	const UFeatureFinderSettings* Settings = GetDefault<UFeatureFinderSettings>();
	if (!Settings)
	{
		return;
	}

	CachedReferences.Reset();
	
	TSet<const UFeatureTypeConfig*> CachedFeatures;
	CachedFeatures.Reserve(Settings->FeatureTypes.Num());
	
	Features.Reset(Settings->FeatureTypes.Num());
	
	for (const TSoftObjectPtr<const UFeatureTypeConfig>& FeatureTypePtr : Settings->FeatureTypes)
	{
		const UFeatureTypeConfig* FeatureType = FeatureTypePtr.LoadSynchronous();
		if (!FeatureType || CachedFeatures.Contains(FeatureType))
		{
			continue;
		}

		CachedFeatures.Add(FeatureType);
		
		TSharedPtr<FFeature> FeaturePtr = MakeShareable(new FFeature);
		FeaturePtr->Type = FeatureType;

		UpdateFeatureInstances(FeaturePtr);
		Features.Add(FeaturePtr);
	}

	OnFeaturesUpdated.Broadcast();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureFinderSubsystem::TryRecacheFeature(const UFeatureTypeConfig* FeatureType)
{
	if (const TSharedPtr<FFeature>& FeaturePtr = GetFeature(FeatureType))
	{
		UpdateFeatureInstances(FeaturePtr);
		OnFeaturesUpdated.Broadcast();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<FFeature> UFeatureFinderSubsystem::GetLastSelectedFeature() const
{
	const UFeatureFinderUserSettings* FeatureFinderUserSettings = GetDefault<UFeatureFinderUserSettings>();
	const UFeatureTypeConfig* LastSelectedFeatureType = FeatureFinderUserSettings ? FeatureFinderUserSettings->LastSelectedFeatureType.Get() : nullptr;
	
	TSharedPtr<FFeature> LastSelectedFeature = GetFeature(LastSelectedFeatureType);
	if (LastSelectedFeature.IsValid())
	{
		return LastSelectedFeature;
	}

	return !Features.IsEmpty() ? Features[0] : nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureFinderSubsystem::SetLastSelectedFeature(const TSharedPtr<FFeature>& FeaturePtr) const
{
	const FFeature* Feature = FeaturePtr.Get();
	const UFeatureTypeConfig* Type = Feature ? Feature->Type.Get() : nullptr;
	UFeatureFinderUserSettings* FeatureFinderUserSettings = GetMutableDefault<UFeatureFinderUserSettings>();
	if (Type && FeatureFinderUserSettings)
	{
		FeatureFinderUserSettings->LastSelectedFeatureType = Type;
		FeatureFinderUserSettings->SaveConfig();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureFinderSubsystem::UpdateFeatureInstances(const TSharedPtr<FFeature>& FeaturePtr)
{
	FFeature* Feature = FeaturePtr.Get();
	const UFeatureTypeConfig* Type = Feature ? Feature->Type.Get() : nullptr;
	if (!Feature || !Type)
	{
		return;
	}
	
	Feature->Instances.Reset(Type->Features.Num());

	// Copy manual attributes from features defined in the FeatureTypeConfig.
	for (const FEditableFeatureInstance& EditableInstance : Type->Features)
	{
		Feature->Instances.Add(MakeShareable(new FFeatureInstance(EditableInstance.ToFeatureInstance())));
	}
	
	// Copy manual attributes from features defined in the FeaturesTable.
	if (Type->FeaturesTable)
	{
		const TArray<FName> RowNames = Type->FeaturesTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			if (uint8* RowData = Type->FeaturesTable->FindRowUnchecked(RowName))
			{
				FFeatureInstance FeatureInstance = Type->MakeFeatureInstance();
				for (FFeatureAttributeValue& Attribute : FeatureInstance.Attributes)
				{
					if (Attribute->IsValid())
					{
						const FProperty* RowProperty = Type->FeaturesTable->FindTableProperty(Attribute->Get().GetName());
						if (RowProperty && RowProperty->IsA<FStructProperty>())
						{
							const FStructProperty* StructProperty = CastFieldChecked<FStructProperty>(RowProperty);
							if (StructProperty->Struct && StructProperty->Struct->IsChildOf<FFeatureAttributeValueInternal>())
							{
								const FFeatureAttributeValueInternal* InternalAttribute = StructProperty->ContainerPtrToValuePtr<FFeatureAttributeValueInternal>(RowData, 0);
								if (InternalAttribute && InternalAttribute->GetType() == Attribute->Get().GetType())
								{
									Attribute->GetMutable().CopyFrom(InternalAttribute);
								}
							}
						}
					}
				}

				Feature->Instances.Add(MakeShared<FFeatureInstance>(FeatureInstance));
			}
		}
	}

	const TArray<FName> GeneratedAttributeNames = Type->GetGeneratedAttributeNames();
	for (const FName& GeneratedAttributeName : GeneratedAttributeNames)
	{
		const FFeatureAttributeConfig& GeneratedAttribute = Type->GetAttribute(GeneratedAttributeName);
		for (const TSharedPtr<FFeatureInstance>& InstancePtr : Feature->Instances)
		{
			if (FFeatureInstance* Instance = InstancePtr.Get())
			{
				Instance->Attributes.Add(GeneratedAttribute->Get().GenerateValueStruct(*Instance));
			}
		}
	}

	for (const TSharedPtr<FFeatureInstance>& InstancePtr : Feature->Instances)
	{
		if (const FFeatureInstance* Instance = InstancePtr.Get())
		{
			for (const FFeatureAttributeValue& Attribute : Instance->Attributes)
			{
				if (Attribute->IsValid())
				{
					Attribute->Get().LoadAndCacheReferences(CachedReferences);
				}
			}
		}
	}
}
