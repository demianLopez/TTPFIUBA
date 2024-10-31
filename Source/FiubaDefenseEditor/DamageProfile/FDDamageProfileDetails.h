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

	TArray<EFDWeaponDamageType> BuildTowerDamages() const;
	TArray<EFDMonsterArmorType> BuildMonsterArmors() const;

	FText GetDamageTypeDisplayName(EFDWeaponDamageType DamageType) const;
	FText GetArmorTypeDisplayName(EFDMonsterArmorType ArmorType) const;

	void OnValueCommitted(float Value, ETextCommit::Type, EFDMonsterArmorType ArmorType, EFDWeaponDamageType DamageType);

	UFDDamageProfileSettings* GetProfile() const;
};
