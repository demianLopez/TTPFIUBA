// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDDamageTypes.h"
#include "Engine/DeveloperSettings.h"
#include "FDDamageProfileSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig, meta = (DisplayName = "Damage Profiles"))
class FIUBADEFENSE_API UFDDamageProfileSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	static const UFDDamageProfileSettings* Get();
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly)
	TMap<EFDMonsterArmorType, FText> ArmorTypesDisplayName;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly)
	TMap<EFDTowerDamageType, FText> DamageTypesDisplayName;

	float GetDamageMultiplierBetween(EFDTowerDamageType DamageType, EFDMonsterArmorType ArmorType) const;

#if WITH_EDITOR
	void SetDamageMultiplierBetween(EFDTowerDamageType DamageType, EFDMonsterArmorType ArmorType, float NewDamageValue);
#endif

protected:

	int32 GetDamageArmorHash(EFDTowerDamageType DamageType, EFDMonsterArmorType ArmorType) const;
	
	UPROPERTY(Config)
	TMap<int32, float> DamageMultiplierMap;
};
