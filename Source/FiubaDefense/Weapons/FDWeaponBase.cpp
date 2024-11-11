// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FDWeaponBase.h"

#include "Data/FDWeaponDataAsset.h"

// Sets default values
AFDWeaponBase::AFDWeaponBase()
{


}

void AFDWeaponBase::InitWithData(const UFDWeaponDataAsset* InWeaponDataAsset)
{
	WeaponData = InWeaponDataAsset;

	FireRate = WeaponData->BaseFireRate;
}

