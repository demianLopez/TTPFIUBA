// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Shop/Widget_ShopItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/FDWeaponDataAsset.h"
#include "Game/FDShopComponent.h"

void UWidget_ShopItem::InitWithItem(TWeakPtr<FDShopItem> InShopItem)
{
	ShopItem = InShopItem;
	
	if(InShopItem.Pin()->bAvailable)
	{
		SetAsEnabled();

		const UFDWeaponDataAsset* WeaponDataAsset = ShopItem.Pin()->WeaponData.Get();
		if(WeaponDataAsset->Icon.IsValid())
		{
			Image_WeaponIcon->SetBrushFromTexture(WeaponDataAsset->Icon.Get());
		}
	
		TextBlock_WeaponName->SetText(WeaponDataAsset->WeaponName);
	}
	else
	{
		SetAsDisabled();
	}

}

void UWidget_ShopItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	OnItemPreviewDelegate.Broadcast(ShopItem);
}

void UWidget_ShopItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnItemPreviewDelegate.Broadcast(nullptr);
}

FReply UWidget_ShopItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if(InMouseEvent.GetPressedButtons().Contains(EKeys::LeftMouseButton))
	{
		OnItemClicked.Broadcast(ShopItem);
	}

	return FReply::Handled();
}
