// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Shop/Widget_Shop.h"

#include "Widget_ItemPreview.h"
#include "Components/UniformGridPanel.h"
#include "Game/FDShopComponent.h"
#include "Input/CommonUIInputTypes.h"
#include "Player/FDPlayerController.h"
#include "Widget_ShopItem.h"
#include "Components/TextBlock.h"
#include "Game/FDShopComponent.h"
#include "Player/FDPlayerState.h"

void UWidget_Shop::RegisterInputActions()
{
	Super::RegisterInputActions();

	{
		FBindUIActionArgs InputActionArgs(InputActionCloseShop, false, FSimpleDelegate::CreateUObject(this, &ThisClass::DeactivateWidget));
		InputActionArgs.InputMode = ECommonInputMode::Game;	
		RegisterUIActionBinding(InputActionArgs);
	}
}

void UWidget_Shop::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AFDPlayerState* OwningPlayerState = GetOwningPlayerState<AFDPlayerState>();

	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MaximumFractionalDigits = 0;
	FormattingOptions.MinimumFractionalDigits = 0;
	
	TextBlock_PlayerGold->SetText(FText::AsNumber(OwningPlayerState->GetGold(), &FormattingOptions));
}

void UWidget_Shop::NativeOnActivated()
{
	Super::NativeOnActivated();

	AFDPlayerController* OwningFDPlayer = GetOwningPlayer<AFDPlayerController>();
	CachedShopComponent = OwningFDPlayer->GetShopComponent();
	
	CachedShopComponent->OnShopRefreshed.AddUObject(this, &ThisClass::RefreshShop);
	
	RefreshShop();
}

void UWidget_Shop::NativeOnDeactivated()
{
	if(CachedShopComponent.IsValid())
	{
		CachedShopComponent->OnShopRefreshed.RemoveAll(this);
	}

	CachedShopComponent = nullptr;
	
	Super::NativeOnDeactivated();
}

void UWidget_Shop::RefreshShop()
{
	if(!CachedShopComponent.IsValid())
		return;

	const auto& SellingAssets = CachedShopComponent->GetSellingAssets();

	UniformGridPanel_ShopItems->ClearChildren();

	int32 WidgetIndex = 0;
	for(TSharedPtr<FDShopItem> WeaponData : SellingAssets)
	{
		UWidget_ShopItem* ShopItem = CreateWidget<UWidget_ShopItem>(GetOwningPlayer(), ShopItemClass);
		ShopItem->OnItemPreviewDelegate.AddUObject(this, &ThisClass::OnItemPreview);
		ShopItem->OnItemClicked.AddUObject(this, &ThisClass::OnItemClicked);
		
		const int32 ColNumber = WidgetIndex / NumberOfCols;
		const int32 RowNumber = WidgetIndex % NumberOfCols;

		UniformGridPanel_ShopItems->AddChildToUniformGrid(ShopItem, ColNumber, RowNumber);
		ShopItem->InitWithItem(WeaponData);
		WidgetIndex++;
	}
}

void UWidget_Shop::OnItemPreview(TWeakPtr<FDShopItem> Item)
{
	if(Item.IsValid() && Item.Pin()->bAvailable)
	{
		WidgetItemPreview->DisplayPreview(Item);
		WidgetItemPreview->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		WidgetItemPreview->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWidget_Shop::OnItemClicked(TWeakPtr<FDShopItem> Item)
{
	if(CachedShopComponent.IsValid())
	{
		CachedShopComponent->TryBuyItem(Item.Pin());
	}
}
