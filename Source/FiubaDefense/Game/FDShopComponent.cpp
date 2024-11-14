// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FDShopComponent.h"

#include "FDGameMode.h"
#include "Data/FDWeaponDataAsset.h"
#include "Engine/AssetManager.h"
#include "Player/FDPlayerController.h"
#include "Player/FDPlayerState.h"

FDShopItem::FDShopItem(int32 Index) : bAvailable(false), Index(Index)
{

}

// Sets default values for this component's properties
UFDShopComponent::UFDShopComponent()
{

}

void UFDShopComponent::BeginPlay()
{
	Super::BeginPlay();

	SellingItems.Reserve(ShopItemsPerRefresh);
	for(int ItemIndex = 0; ItemIndex < ShopItemsPerRefresh; ItemIndex++)
	{
		TSharedPtr<FDShopItem> ShopItem = MakeShareable(new FDShopItem(ItemIndex));
		SellingItems.Add(ShopItem);
	}

	UAssetManager& AssetManager = UAssetManager::Get();

	FPrimaryAssetType WeaponType("FDWeaponDataAsset");

	TArray<FAssetData> WeaponAssetData;
	AssetManager.GetPrimaryAssetDataList(WeaponType, WeaponAssetData);

	TArray<FPrimaryAssetId> AssetIds;
	AssetIds.Reserve(WeaponAssetData.Num());

	for(const FAssetData& AssetData : WeaponAssetData)
	{
		AssetIds.Add(AssetData.GetPrimaryAssetId());
	}
	
	WeaponStreamableHandle = AssetManager.LoadPrimaryAssets(AssetIds, {"Game"}
		, FStreamableDelegate::CreateUObject(this, &ThisClass::OnWeaponLoadedComplete));
}

void UFDShopComponent::OnWeaponLoadedComplete()
{
	if(!WeaponStreamableHandle.IsValid())
		return;
	
	TArray<UObject*> LoadedWeaponsAssets;
	WeaponStreamableHandle->GetLoadedAssets(LoadedWeaponsAssets);

	WeaponDataAssets.Reserve(LoadedWeaponsAssets.Num());
	
	for(const UObject* LoadedWeaponAsset : LoadedWeaponsAssets)
	{
		const UFDWeaponDataAsset* WeaponDataAsset = Cast<const UFDWeaponDataAsset>(LoadedWeaponAsset);
		if(!IsValid(WeaponDataAsset))
			continue;
		
		WeaponDataAssets.Add(WeaponDataAsset);
	}
}

void UFDShopComponent::RefreshShop()
{	
	for(int32 i = 0; i < ShopItemsPerRefresh; i++)
	{
		TSharedPtr<FDShopItem> Item = SellingItems[i];

		Item->bAvailable = true;
		
		const int32 RandomUpdateIndex = FMath::RandRange(1, WeaponDataAssets.Num()) - 1;
		Item->WeaponData = WeaponDataAssets[RandomUpdateIndex];
	}

	OnShopRefreshed.Broadcast();
}

bool UFDShopComponent::TryBuyItem(TSharedPtr<FDShopItem> ShopItem)
{
	if(!ShopItem->bAvailable)
		return false;
	
	const UFDWeaponDataAsset* ItemWeaponData = ShopItem->WeaponData.Get();

	AFDPlayerController* OwningFDPlayerController = GetOwner<AFDPlayerController>();
	AFDPlayerState* OwningPlayerState = OwningFDPlayerController->GetPlayerState<AFDPlayerState>();

	if(OwningPlayerState->GetGold() < ItemWeaponData->BasePrice)
		return false;

	OwningPlayerState->SubstractGold(ItemWeaponData->BasePrice);
	ShopItem->bAvailable = false;

	AFDGameMode* FDGameMode = GetWorld()->GetAuthGameMode<AFDGameMode>();
	FDGameMode->CreateWeaponForPlayer(OwningFDPlayerController, ItemWeaponData);

	OnShopRefreshed.Broadcast();

	return true;
}
