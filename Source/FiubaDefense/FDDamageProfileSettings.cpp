// Fill out your copyright notice in the Description page of Project Settings.


#include "FDDamageProfileSettings.h"

const UFDDamageProfileSettings* UFDDamageProfileSettings::Get()
{
	return GetDefault<UFDDamageProfileSettings>();
}

float UFDDamageProfileSettings::GetDamageMultiplierBetween(EFDTowerDamageType DamageType, EFDMonsterArmorType MonsterArmorTypes) const
{
	const int32 DamageArmorHash = GetDamageArmorHash(DamageType, MonsterArmorTypes);
	const float* Multiplier = DamageMultiplierMap.Find(DamageArmorHash);

	if(Multiplier == nullptr)
		return 1.0f;

	return *Multiplier;
}

int32 UFDDamageProfileSettings::GetDamageArmorHash(EFDTowerDamageType DamageType, EFDMonsterArmorType ArmorType) const
{
	const int32 DamageTypeHash = static_cast<uint32>(DamageType);
	const int32 ArmorTypeHash = static_cast<uint32>(ArmorType) << 16;	
	
	return DamageTypeHash | ArmorTypeHash;
}

#if WITH_EDITOR
void UFDDamageProfileSettings::SetDamageMultiplierBetween(EFDTowerDamageType DamageType, EFDMonsterArmorType ArmorType, float NewDamageValue)
{
	const int32 DamageArmorHash = GetDamageArmorHash(DamageType, ArmorType);

	float& DamageMultiplier = DamageMultiplierMap.FindOrAdd(DamageArmorHash);
	DamageMultiplier = NewDamageValue;
}
#endif

