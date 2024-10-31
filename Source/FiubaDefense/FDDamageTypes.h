// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFDMonsterArmorType : uint8
{
	Unarmed,
	LightArmor,
	MediumArmor,
	HeavyArmor,
	Divine
};

UENUM(BlueprintType)
enum class EFDWeaponDamageType : uint8
{
	Normal,
	Pierce ,
	Siege,
	Hero,
	Chaos,
	Magic
};