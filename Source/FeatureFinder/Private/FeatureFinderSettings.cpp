#include "FeatureFinderSettings.h"
#include "FeatureFinderSubsystem.h"

#if WITH_EDITOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UFeatureFinderSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty->GetName() == GET_MEMBER_NAME_CHECKED(UFeatureFinderSettings, FeatureTypes))
	{
		UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
		FeatureFinderSubsystem.RecacheAllFeatures();
	}
}
#endif // WITH_EDITOR
