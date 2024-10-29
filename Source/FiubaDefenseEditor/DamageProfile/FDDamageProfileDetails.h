#pragma once

#include "IDetailCustomization.h"
#include "FDDamageTypes.h"

class UFDDamageProfileSettings;

class FFDDamageProfileDetails : public IDetailCustomization
{
public:

	FFDDamageProfileDetails(){};

	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:

	TArray<EFDTowerDamageType> BuildTowerDamages() const;
	TArray<EFDMonsterArmorType> BuildMonsterArmors() const;

	FText GetDamageTypeDisplayName(EFDTowerDamageType DamageType) const;
	FText GetArmorTypeDisplayName(EFDMonsterArmorType ArmorType) const;

	void OnValueCommitted(float Value, ETextCommit::Type, EFDMonsterArmorType ArmorType, EFDTowerDamageType DamageType);

	UFDDamageProfileSettings* GetProfile() const;
};
