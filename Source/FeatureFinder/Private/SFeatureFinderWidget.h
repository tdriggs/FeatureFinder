#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/SCompoundWidget.h"

struct FFeature;
struct FFeatureInstance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SFeatureFinderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFeatureFinderWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	void HandleOnFeaturesUpdated();
	
	TSharedRef<SWidget> CreateHeader();
	TSharedRef<SWidget> GenerateFeatureWidget(TSharedPtr<FFeature> FeaturePtr) const;
	void HandleOnFeatureSelectionChanged(TSharedPtr<FFeature> InSelectedFeaturePtr, ESelectInfo::Type SelectionType);
	FText GetSelectedFeatureText() const;
	
	TSharedRef<SWidget> CreateContent();
	TSharedRef<ITableRow> GenerateFeatureInstanceRowWidget(TSharedPtr<FFeatureInstance> FeatureInstancePtr, const TSharedRef<STableViewBase>& OwnerTable) const;

	void HandleOnFilterTextChanged(const FText& InFilterText);
	FReply HandleOnRefreshClicked();

	void PopulateAttributeView();
	void UpdateVisibleFeatureInstances();

	int32 CalculateInitialColumnWidth(const FName& AttributeName) const;
	
	TSharedPtr<SComboBox<TSharedPtr<FFeature>>> FeatureComboBox;
	TSharedPtr<SSearchBox> FilterSearchBox;

	TSharedPtr<SHeaderRow> FeaturesHeaderRow;
	TSharedPtr<SListView<TSharedPtr<FFeatureInstance>>> FeaturesListView;

	TSharedPtr<FFeature> SelectedFeaturePtr;
	TArray<TSharedPtr<FFeatureInstance>> VisibleFeatureInstances;
	
	FDelegateHandle OnFeaturesUpdatedHandle;
	bool bBlockUpdateAttributeGrid = false;
};
