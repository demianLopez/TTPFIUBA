#include "FDDamageProfileDetails.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "FDDamageProfileSettings.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "FDDamageProfileDetails"

TSharedRef<IDetailCustomization> FFDDamageProfileDetails::MakeInstance()
{
	return MakeShareable( new FFDDamageProfileDetails());
}

void FFDDamageProfileDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& DamageInteractionCategory = DetailBuilder.EditCategory("Damage Interactions");

	TSharedPtr<SUniformGridPanel> UniformGridPanel;
	DamageInteractionCategory.AddCustomRow(LOCTEXT("DamageProfileDamageTable", "DamageTable"))
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.AutoHeight()
		[
			SAssignNew(UniformGridPanel, SUniformGridPanel)
		]

	];

	TArray<EFDWeaponDamageType> DamageTypes = BuildTowerDamages();
	TArray<EFDMonsterArmorType> ArmorTypes = BuildMonsterArmors();

	int32 DamageTypeIndex = 1;
	for(EFDWeaponDamageType DamageType : DamageTypes)
	{
		UniformGridPanel->AddSlot(0, DamageTypeIndex)
		[
			SNew(STextBlock).Text(GetDamageTypeDisplayName(DamageType))
		];
		DamageTypeIndex++;
	}
	
	int32 ArmorTypeIndex = 1;
	for(EFDMonsterArmorType ArmorType : ArmorTypes)
	{
		UniformGridPanel->AddSlot(ArmorTypeIndex, 0)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text(GetArmorTypeDisplayName(ArmorType))
		];
		ArmorTypeIndex++;
	}

	UFDDamageProfileSettings* DamageProfile = GetProfile();
	DamageTypeIndex = 1;
	for(EFDWeaponDamageType DamageType : DamageTypes)
	{
		ArmorTypeIndex = 1;
		for(EFDMonsterArmorType ArmorType : ArmorTypes)
		{
			UniformGridPanel->AddSlot(ArmorTypeIndex, DamageTypeIndex)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox).Padding(FMargin(5.0f, 2.0f))
				.WidthOverride(100.0f)
				[
					SNew(SSpinBox<float>)
					.MinValue(0.0f).MaxValue(10.0f)
					.MinSliderValue(0.0f).MaxSliderValue(10.0f)
					.Justification(ETextJustify::Center)
					.Delta(0.01f).MinFractionalDigits(3).MaxFractionalDigits(3).AlwaysUsesDeltaSnap(true)
					.Value(DamageProfile->GetDamageMultiplierBetween(DamageType, ArmorType)).LinearDeltaSensitivity(10000.0f)
					.OnValueCommitted(this, &FFDDamageProfileDetails::OnValueCommitted, ArmorType, DamageType)
				]
			];
			ArmorTypeIndex++;
		}
		DamageTypeIndex++;
	}
}

void FFDDamageProfileDetails::OnValueCommitted(float Value, ETextCommit::Type, EFDMonsterArmorType ArmorType, EFDWeaponDamageType DamageType)
{
	UFDDamageProfileSettings* DamageProfile = GetProfile();
	DamageProfile->SetDamageMultiplierBetween(DamageType, ArmorType, Value);
	DamageProfile->TryUpdateDefaultConfigFile();
}

TArray<EFDWeaponDamageType> FFDDamageProfileDetails::BuildTowerDamages() const
{
	TArray<EFDWeaponDamageType> DamageTypes;
	
	//		const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/SourceFilteringCore.EFilterSetMode"), true);
	const UEnum* TowerDamageTypesEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFDWeaponDamageType"));
	for(int32 idx = 0; idx < TowerDamageTypesEnum->NumEnums() - 1; ++idx)
	{
		EFDWeaponDamageType ArmorType = static_cast<EFDWeaponDamageType>(TowerDamageTypesEnum->GetValueByIndex(idx));
		DamageTypes.Add(ArmorType);		
	}

	return DamageTypes;
}

TArray<EFDMonsterArmorType> FFDDamageProfileDetails::BuildMonsterArmors() const
{
	TArray<EFDMonsterArmorType> ArmorTypes;
	
	const UEnum* MonsterArmorTypesEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFDMonsterArmorType"));	
	for(int32 idx = 0; idx < MonsterArmorTypesEnum->NumEnums() - 1; ++idx)
	{
		EFDMonsterArmorType ArmorType = static_cast<EFDMonsterArmorType>(MonsterArmorTypesEnum->GetValueByIndex(idx));
		ArmorTypes.Add(ArmorType);		
	}

	return ArmorTypes;
}

FText FFDDamageProfileDetails::GetDamageTypeDisplayName(EFDWeaponDamageType DamageType) const
{
	UFDDamageProfileSettings* DamageProfile = GetProfile();

	FText* CustomDisplayName = DamageProfile->DamageTypesDisplayName.Find(DamageType);
	if(CustomDisplayName != nullptr)
		return *CustomDisplayName;
	
	const UEnum* TowerDamageTypesEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFDWeaponDamageType"));
	return TowerDamageTypesEnum->GetDisplayNameTextByValue(static_cast<int64>(DamageType));
}

FText FFDDamageProfileDetails::GetArmorTypeDisplayName(EFDMonsterArmorType ArmorType) const
{
	UFDDamageProfileSettings* DamageProfile = GetProfile();

	FText* CustomDisplayName = DamageProfile->ArmorTypesDisplayName.Find(ArmorType);
	if(CustomDisplayName != nullptr)
		return *CustomDisplayName;
	
	const UEnum* TowerDamageTypesEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFDMonsterArmorType"));
	return TowerDamageTypesEnum->GetDisplayNameTextByValue(static_cast<int64>(ArmorType));
}
#undef LOCTEXT_NAMESPACE

UFDDamageProfileSettings* FFDDamageProfileDetails::GetProfile() const
{
	return GetMutableDefault<UFDDamageProfileSettings>();
}
