#include "SFeatureInstanceRow.h"
#include "EditorUtilityLibrary.h"
#include "FeatureFinderAttributeTypes.h"
#include "HAL/PlatformApplicationMisc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SFeatureInstanceRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
{
	FeatureInstancePtr = InArgs._FeatureInstance;
	FeatureTypePtr = InArgs._FeatureType;
	
	SMultiColumnTableRow<TSharedPtr<FFeatureInstance>>::Construct(
		FSuperRowType::FArguments()
		.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
		OwnerTable
	);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureInstanceRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	TSharedPtr<SWidget> CellWidget;
	
	if (const FFeatureInstance* FeatureInstance = FeatureInstancePtr.Get())
	{
		for (const FFeatureAttributeValue& Attribute : FeatureInstance->Attributes)
		{
			if (Attribute->IsValid() && Attribute->Get().GetName() == ColumnName)
			{
				switch (Attribute->Get().GetType())
				{
				case EFeatureAttributeType::Text:
					CellWidget = GenerateTextValueWidget(Attribute->Get<FFeatureAttributeValue_Text>());
					break;
				case EFeatureAttributeType::Folder:
					CellWidget = GenerateFolderValueWidget(Attribute->Get<FFeatureAttributeValue_Folder>());
					break;
				case EFeatureAttributeType::Asset:
					CellWidget = GenerateAssetValueWidget(Attribute->Get<FFeatureAttributeValue_Asset>());
					break;
				case EFeatureAttributeType::Property:
					CellWidget = GeneratePropertyValueWidget(Attribute->Get<FFeatureAttributeValue_Property>());
					break;
				case EFeatureAttributeType::Command:
					CellWidget = GenerateCommandValueWidget(Attribute->Get<FFeatureAttributeValue_Command>());
					break;
				default:
					break;
				}

				break;
			}
		}
	}

	if (CellWidget.IsValid())
	{
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(4.0f, 0.0f)
			.MinDesiredHeight(25.0f)
			[
				CellWidget.ToSharedRef()
			];
	}
	
	return SNew(SBox)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 0.0f)
		.MinDesiredHeight(25.0f)
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
			.Text(FText::FromString(TEXT("Invalid")))
		];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureInstanceRow::GenerateTextValueWidget(const FFeatureAttributeValue_Text& Attribute) const
{
	return SNew(STextBlock)
		.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
		.Text(FText::FromString(Attribute.Text));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureInstanceRow::GenerateFolderValueWidget(const FFeatureAttributeValue_Folder& Attribute) const
{
	FString PlatformDirectory;
	const bool bSuccess = FPackageName::TryConvertGameRelativePackagePathToLocalPath(Attribute.Folder, PlatformDirectory);
	PlatformDirectory = FPaths::ConvertRelativePathToFull(PlatformDirectory);
	if (Attribute.Folder.IsEmpty() || !bSuccess || !FPaths::DirectoryExists(PlatformDirectory))
	{
		return SNew(STextBlock)
		.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
		.Text(FText::FromString(TEXT("None")));
	}
	
	return SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &SFeatureInstanceRow::FolderValue_OnBrowseClicked, Attribute)
			.ToolTipText(FText::FromString(TEXT("Open Asset in Content Browser")))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Search"))
			]
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SSpacer)
			.Size(FVector2D(2.0f,1.0f))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(FPaths::GetPathLeaf(Attribute.Folder)))
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
			.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureInstanceRow::GenerateAssetValueWidget(const FFeatureAttributeValue_Asset& Attribute) const
{
	if (!Attribute.Asset)
	{
		return SNew(STextBlock)
		.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
		.Text(FText::FromString(TEXT("None")));
	}
	
	return SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &SFeatureInstanceRow::AssetValue_OnBrowseClicked, Attribute)
			.ToolTipText(FText::FromString(TEXT("Open Asset in Content Browser")))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Search"))
			]
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &SFeatureInstanceRow::AssetValue_OnEditClicked, Attribute)
			.ToolTipText(FText::FromString(TEXT("Open Asset in editor")))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Edit"))
			]
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SSpacer)
			.Size(FVector2D(2.0f,1.0f))
		]
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(GetNameSafe(Attribute.Asset.Get())))
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
			.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureInstanceRow::GeneratePropertyValueWidget(const FFeatureAttributeValue_Property& Attribute) const
{
	return SNew(STextBlock)
		.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
		.Text_Lambda([Attribute]() { return FText::FromString(Attribute.ToString()); });
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<SWidget> SFeatureInstanceRow::GenerateCommandValueWidget(const FFeatureAttributeValue_Command& Attribute) const
{
	if (Attribute.Command.IsEmpty())
	{
		return SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
			.Text(FText::FromString(TEXT("Invalid")));
	}

	FString DisplayCommand = Attribute.Command;
	if (const UFeatureTypeConfig* FeatureType = FeatureTypePtr.Get())
	{
		const FFeatureAttributeConfig& AttributeConfig = FeatureType->GetAttribute(Attribute.GetName());
		if (AttributeConfig->IsValid() && AttributeConfig->Get().GetType() == EFeatureAttributeType::Command)
		{
			const FFeatureAttributeConfig_Command& CommandConfig = AttributeConfig->Get<FFeatureAttributeConfig_Command>();
			if (CommandConfig.bUseOverrideDisplayCommand)
			{
				DisplayCommand = CommandConfig.OverrideDisplayCommand;
			}
		}
	}
	
	
	return SNew(SBox)
		.HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.OnClicked(this, &SFeatureInstanceRow::CommandValue_OnCommandClicked, Attribute)
				.IsEnabled(this, &SFeatureInstanceRow::CommandValue_IsEnabled, Attribute)
				.ContentPadding(4.0f)
				.ForegroundColor(FSlateColor::UseForeground())
				[
					SNew(STextBlock)
					.Text(FText::FromString(DisplayCommand))
					.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				]
			]
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SSpacer)
				.Size(FVector2D(2.0f,1.0f))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
				.OnClicked(this, &SFeatureInstanceRow::CommandValue_OnCopyClicked, Attribute)
				.ToolTipText(FText::FromString(TEXT("Copy the command to the clipboard")))
				.ContentPadding(4.0f)
				.ForegroundColor(FSlateColor::UseForeground())
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("GenericCommands.Copy"))
				]
			]
		];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SFeatureInstanceRow::CommandValue_IsEnabled(const FFeatureAttributeValue_Command Attribute) const
{
	const UFeatureTypeConfig* FeatureType = FeatureTypePtr.Get();
	if (!FeatureType || Attribute.Command.IsEmpty() || !GEditor)
	{
		return false;
	}
	
	const FFeatureAttributeConfig& AttributeConfig = FeatureType->GetAttribute(Attribute.GetName());
	if (!AttributeConfig->IsValid() || AttributeConfig->Get().GetType() != EFeatureAttributeType::Command)
	{
		return false;
	}

	const FFeatureAttributeConfig_Command& CommandConfig = AttributeConfig->Get<FFeatureAttributeConfig_Command>();
	return !CommandConfig.bOnlyRunInPIE || GEditor->IsPlayingSessionInEditor();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply SFeatureInstanceRow::FolderValue_OnBrowseClicked(const FFeatureAttributeValue_Folder Attribute) const
{
	FString Folder = Attribute.Folder;
	FPaths::NormalizeFilename(Folder);
	Folder.RemoveFromEnd(TEXT("/"));
	
	TArray<FString> Folders;
	Folders.Add(Folder);
	UEditorUtilityLibrary::SyncBrowserToFolders(Folders);
	
	return FReply::Handled();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply SFeatureInstanceRow::AssetValue_OnBrowseClicked(const FFeatureAttributeValue_Asset Attribute) const
{
	if (Attribute.Asset && GEditor)
	{
		TArray<UObject*> ObjectList;
		ObjectList.Add(Attribute.Asset.Get());
		GEditor->SyncBrowserToObjects(ObjectList);
	}

	return FReply::Handled();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply SFeatureInstanceRow::AssetValue_OnEditClicked(const FFeatureAttributeValue_Asset Attribute) const
{
	if (Attribute.Asset && GEditor)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OpenEditorForAsset(Attribute.Asset.Get());
	}

	return FReply::Handled();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply SFeatureInstanceRow::CommandValue_OnCommandClicked(const FFeatureAttributeValue_Command Attribute) const
{
	const UFeatureTypeConfig* FeatureType = FeatureTypePtr.Get();
	if (!FeatureType || Attribute.Command.IsEmpty() || !GEditor)
	{
		return FReply::Handled();
	}
	
	const FFeatureAttributeConfig& AttributeConfig = FeatureType->GetAttribute(Attribute.GetName());
	if (!AttributeConfig->IsValid() || AttributeConfig->Get().GetType() != EFeatureAttributeType::Command)
	{
		return FReply::Handled();
	}

	const FFeatureAttributeConfig_Command& CommandConfig = AttributeConfig->Get<FFeatureAttributeConfig_Command>();

	auto RunCommand = [&](UWorld* World = nullptr)
	{
		const bool bSuccess = IConsoleManager::Get().ProcessUserConsoleInput(*Attribute.Command, *GLog, World);
		if (!bSuccess)
		{
			if (APlayerController* TargetPC = World ? World->GetFirstPlayerController() : nullptr)
			{
				TargetPC->ConsoleCommand(Attribute.Command, true);
			}
			else
			{
				GEngine->Exec(World, *Attribute.Command);
			}
		}
	};

	if (!GEditor->IsPlayingSessionInEditor())
	{
		if (!CommandConfig.bOnlyRunInPIE)
		{
			RunCommand();
		}
		
		return FReply::Handled();
	}
	
	bool bHasRunOnStandalone = false;
	bool bHasRunOnClient = false;
	for (const FWorldContext& WorldContext : GEditor->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (World && WorldContext.WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Standalone:
				if (!bHasRunOnStandalone || !CommandConfig.bOnlyRunOnFirstPIEStandalone)
				{
					RunCommand(World);
					bHasRunOnStandalone = true;
				}
				break;
				
			case NM_DedicatedServer:
			case NM_ListenServer:
				if (CommandConfig.bRunOnPIEServers)
				{
					RunCommand(World);
				}
				break;
				
			case NM_Client:
				if (!bHasRunOnClient || !CommandConfig.bOnlyRunOnFirstPIEClient)
				{
					RunCommand(World);
					bHasRunOnClient = true;
				}
				break;
				
			default:
				break;
			}
		}
	}
	
	return FReply::Handled();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FReply SFeatureInstanceRow::CommandValue_OnCopyClicked(const FFeatureAttributeValue_Command Attribute) const
{
	FPlatformApplicationMisc::ClipboardCopy(*Attribute.Command);
	return FReply::Handled();
}
