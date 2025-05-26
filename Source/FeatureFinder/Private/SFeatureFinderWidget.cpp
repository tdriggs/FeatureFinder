#include "SFeatureFinderWidget.h"
#include "FeatureFinderAttributeTypes.h"
#include "FeatureFinderSubsystem.h"
#include "Fonts/FontMeasure.h"
#include "SFeatureInstanceRow.h"
#include "Widgets/Input/SSearchBox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureFinderWidget::Construct(const FArguments& InArgs)
{
	UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
	
	FeatureFinderSubsystem.OnFeaturesUpdated.Remove(OnFeaturesUpdatedHandle);
	OnFeaturesUpdatedHandle = FeatureFinderSubsystem.OnFeaturesUpdated.AddRaw(this, &SFeatureFinderWidget::HandleOnFeaturesUpdated);

	SelectedFeaturePtr = FeatureFinderSubsystem.GetLastSelectedFeature();
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		[
			CreateHeader()
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		[
			SNew(SSeparator)
			.Orientation(Orient_Horizontal)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			CreateContent()
		]
	];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureFinderWidget::CreateHeader()
{
	UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
	
	return SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(8.0f)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &SFeatureFinderWidget::HandleOnRefreshClicked)
			.ToolTipText(FText::FromString(TEXT("Copy the command to the clipboard")))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Refresh"))
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		.AutoWidth()
		.Padding(0.0f, 8.0f)
		[
			SAssignNew(FeatureComboBox, SComboBox<TSharedPtr<FFeature>>)
			.OptionsSource(&FeatureFinderSubsystem.GetFeatures())
			.OnGenerateWidget(this, &SFeatureFinderWidget::GenerateFeatureWidget)
			.OnSelectionChanged(this, &SFeatureFinderWidget::HandleOnFeatureSelectionChanged)
			.InitiallySelectedItem(SelectedFeaturePtr)
			.ContentPadding(FMargin(4.0, 1.0f))
			[
				SNew(STextBlock)
				.Text(this, &SFeatureFinderWidget::GetSelectedFeatureText)
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
				.MinDesiredWidth(100.0f)
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		.AutoWidth()
		.FillWidth(1.0f)
		.Padding(8.0f)
		[
			SAssignNew(FilterSearchBox, SSearchBox)
			.OnTextChanged(this, &SFeatureFinderWidget::HandleOnFilterTextChanged)
		];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureFinderWidget::HandleOnFeaturesUpdated()
{
	const UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
	
	bBlockUpdateAttributeGrid = true;
	
	if (SComboBox<TSharedPtr<FFeature>>* ComboBox = FeatureComboBox.Get())
	{
		ComboBox->RefreshOptions();
		ComboBox->SetSelectedItem(FeatureFinderSubsystem.GetLastSelectedFeature());
	}
	else
	{
		HandleOnFeatureSelectionChanged(FeatureFinderSubsystem.GetLastSelectedFeature(), ESelectInfo::Direct);
	}
	
	bBlockUpdateAttributeGrid = false;
	PopulateAttributeView();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureFinderWidget::GenerateFeatureWidget(TSharedPtr<FFeature> FeaturePtr) const
{
	return SNew(STextBlock)
		.Text_Lambda([FeaturePtr]
		{
			const FFeature* Feature = FeaturePtr.Get();
			const UFeatureTypeConfig* Type = Feature ? Feature->Type.Get() : nullptr;
			return FText::FromString(Type ? Type->DisplayName : TEXT("Invalid"));
		})
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureFinderWidget::HandleOnFeatureSelectionChanged(TSharedPtr<FFeature> InSelectedFeaturePtr, ESelectInfo::Type SelectionType)
{
	SelectedFeaturePtr = InSelectedFeaturePtr;
	
	const UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
	FeatureFinderSubsystem.SetLastSelectedFeature(SelectedFeaturePtr);

	PopulateAttributeView();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FText SFeatureFinderWidget::GetSelectedFeatureText() const
{
	const FFeature* Feature = SelectedFeaturePtr.Get();
	if (const UFeatureTypeConfig* Type = Feature ? Feature->Type.Get() : nullptr)
	{
		return FText::FromString(Type->DisplayName);
	}

	return FText::FromString(TEXT("Invalid"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureFinderWidget::CreateContent()
{
	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(12.0f, 12.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));
	
	FeaturesHeaderRow = SNew(SHeaderRow);

	FeaturesListView = SNew(SListView<TSharedPtr<FFeatureInstance>>)
		.ListItemsSource(&VisibleFeatureInstances)
		.HeaderRow(FeaturesHeaderRow)
		.OnGenerateRow(this, &SFeatureFinderWidget::GenerateFeatureInstanceRowWidget)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::None)
		.AllowOverscroll(EAllowOverscroll::No);
	
	PopulateAttributeView();

	return SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				SNew(SScrollBox)
				.Orientation(Orient_Horizontal)
				.ExternalScrollbar(HorizontalScrollBar)
				+SScrollBox::Slot()
				[
					FeaturesListView.ToSharedRef()
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				VerticalScrollBar
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				HorizontalScrollBar
			]
		];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<ITableRow> SFeatureFinderWidget::GenerateFeatureInstanceRowWidget(TSharedPtr<FFeatureInstance> FeatureInstancePtr, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(SFeatureInstanceRow, OwnerTable)
		.FeatureInstance(FeatureInstancePtr)
		.FeatureType(SelectedFeaturePtr.IsValid() ? SelectedFeaturePtr->Type : nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureFinderWidget::HandleOnFilterTextChanged(const FText& InFilterText)
{
	UpdateVisibleFeatureInstances();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply SFeatureFinderWidget::HandleOnRefreshClicked()
{
	UFeatureFinderSubsystem& FeatureFinderSubsystem = UFeatureFinderSubsystem::GetChecked();
	FeatureFinderSubsystem.RecacheAllFeatures();

	return FReply::Handled();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureFinderWidget::PopulateAttributeView()
{
	if (bBlockUpdateAttributeGrid)
	{
		return;
	}

	if (SHeaderRow* HeaderRow = FeaturesHeaderRow.Get())
	{
		HeaderRow->ClearColumns();
		
		const FFeature* Feature = SelectedFeaturePtr.Get();
		const UFeatureTypeConfig* Type = Feature ? Feature->Type.Get() : nullptr;
		if (Feature && Type)
		{
			for (const FFeatureAttributeConfig& Attribute : Type->Attributes)
			{
				if (!Attribute->IsValid() || Attribute->Get().Name.IsNone() || !Attribute->Get().bIsVisible)
				{
					continue;
				}
				
				HeaderRow->AddColumn(
					SHeaderRow::Column(Attribute->Get().Name)
					.DefaultLabel(FText::FromName(Attribute->Get().Name))
					.ManualWidth(CalculateInitialColumnWidth(Attribute->Get().Name))
					[
						SNew(SBox)
						.Padding(FMargin(0, 4, 0, 4))
						.VAlign(VAlign_Fill)
						[
							SNew(STextBlock)
							.Justification(ETextJustify::Center)
							.Text(FText::FromName(Attribute->Get().Name))
						]
					]
				);
			}
		}
	}

	UpdateVisibleFeatureInstances();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureFinderWidget::UpdateVisibleFeatureInstances()
{
	VisibleFeatureInstances.Reset();

	if (const FFeature* Feature = SelectedFeaturePtr.Get())
	{
		const SSearchBox* SearchBox = FilterSearchBox.Get();
		const FString& FilterString = SearchBox ? SearchBox->GetText().ToString() : FString();
		if (FilterString.IsEmpty())
		{
			VisibleFeatureInstances = Feature->Instances;
		}
		else
		{
			VisibleFeatureInstances.Reserve(Feature->Instances.Num());
			for (const TSharedPtr<FFeatureInstance>& FeatureInstance : Feature->Instances)
			{
				bool bShouldBeVisible = false;
				for (const FFeatureAttributeValue& Attribute : FeatureInstance->Attributes)
				{
					if (Attribute->IsValid() && Attribute->Get().MatchesFilterString(FilterString))
					{
						bShouldBeVisible = true;
						break;
					}
				}

				if (bShouldBeVisible)
				{
					VisibleFeatureInstances.Add(FeatureInstance);
				}
			}
		}
	}

	if (SListView<TSharedPtr<FFeatureInstance>>* ListView = FeaturesListView.Get())
	{
		ListView->RebuildList();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32 SFeatureFinderWidget::CalculateInitialColumnWidth(const FName& AttributeName) const
{
	int32 Width = 0;
	
	const FSlateFontInfo HeaderFont = FCoreStyle::GetDefaultFontStyle("NormalText", 10);
	const FSlateFontInfo CellFont = FCoreStyle::GetDefaultFontStyle("DataTableEditor.CellText", 9);

	const FFeature* Feature = SelectedFeaturePtr.Get();
	const UFeatureTypeConfig* FeatureType = Feature ? Feature->Type.Get() : nullptr;
	if (Feature && FeatureType)
	{
		for (const TSharedPtr<FFeatureInstance>& FeatureInstancePtr : Feature->Instances)
		{
			const FFeatureInstance* FeatureInstance = FeatureInstancePtr.Get();
			if (!FeatureInstance)
			{
				continue;
			}

			for (const FFeatureAttributeValue& Attribute : FeatureInstance->Attributes)
			{
				if (!Attribute->IsValid() || Attribute->Get().GetName() != AttributeName)
				{
					continue;
				}

				bool bValid = false;
				int32 ExtraPadding = 0;
				FString DisplayText = FString();
				
				switch (Attribute->Get().GetType())
				{
				case EFeatureAttributeType::Text:
					{
						DisplayText = Attribute->Get().ToString();
						bValid = true;
						break;
					}
					
				case EFeatureAttributeType::Folder:
					{
						const FString Folder = Attribute->Get().ToString();
						DisplayText = FPaths::GetPathLeaf(Folder);
						
						FString PlatformDirectory;
						const bool bSuccess = FPackageName::TryConvertGameRelativePackagePathToLocalPath(Attribute->Get().ToString(), PlatformDirectory);
						PlatformDirectory = FPaths::ConvertRelativePathToFull(PlatformDirectory);
						bValid = !Folder.IsEmpty() && bSuccess && FPaths::DirectoryExists(PlatformDirectory);

						ExtraPadding = 25;
						
						break;
					}
					
				case EFeatureAttributeType::Asset:
					{
						const FString AssetName = Attribute->Get().ToString();
						DisplayText = AssetName;
						
						bValid = !AssetName.IsEmpty() && AssetName != FString(TEXT("None"));
						
						ExtraPadding = 50;
						
						break;
					}
					
				case EFeatureAttributeType::Property:
					{
						const FString PropertyName = Attribute->Get().ToString();
						DisplayText = PropertyName;
						
						bValid = !PropertyName.IsEmpty() && PropertyName != FString(TEXT("Invalid")) && PropertyName != FString(TEXT("Unsupported"));
						
						break;
					}
					
				case EFeatureAttributeType::Command:
					{
						const FString Command = Attribute->Get().ToString();
						
						const FFeatureAttributeConfig& AttributeConfig = FeatureType->GetAttribute(Attribute->Get().GetName());
						if (AttributeConfig->IsValid() && AttributeConfig->Get().GetType() == EFeatureAttributeType::Command)
						{
							const FFeatureAttributeConfig_Command& CommandConfig = AttributeConfig->Get<FFeatureAttributeConfig_Command>();
							DisplayText = CommandConfig.bUseOverrideDisplayCommand ? CommandConfig.OverrideDisplayCommand : Command;
						}

						bValid = !Command.IsEmpty();

						ExtraPadding = 75;
						
						break;
					}

				default:
					break;
				}

				if (bValid)
				{
					const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
					const FVector2f HeaderTextSize = UE::Slate::CastToVector2f(FontMeasureService->Measure(AttributeName.ToString(), HeaderFont));
					const FVector2f CellTextSize = UE::Slate::CastToVector2f(FontMeasureService->Measure(DisplayText, CellFont));

					static constexpr int32 Margin = 15;
					const int32 HeaderWidth = HeaderTextSize.X + Margin;
					const int32 CellWidth = CellTextSize.X + ExtraPadding + Margin;
					Width = FMath::Max(HeaderWidth, FMath::Max(Width, CellWidth));
				}
			}
		}
	}
	
	return Width;
}
