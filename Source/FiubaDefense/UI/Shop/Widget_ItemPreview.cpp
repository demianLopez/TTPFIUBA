// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Shop/Widget_ItemPreview.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/FDWeaponDataAsset.h"
#include "Game/FDShopComponent.h"

void UWidget_ItemPreview::DisplayPreview(TWeakPtr<FDShopItem> Item)
{
	const UFDWeaponDataAsset* WeaponDataAsset = Item.Pin()->WeaponData.Get();
	TextBlock_WeaponName->SetText(WeaponDataAsset->WeaponName);
}
