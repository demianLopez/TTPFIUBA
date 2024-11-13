// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FDShopComponent.h"

#include "Data/FDWeaponDataAsset.h"
#include "Engine/AssetManager.h"

// Sets default values for this component's properties
UFDShopComponent::UFDShopComponent()
{

}

void UFDShopComponent::BeginPlay()
{
	Super::BeginPlay();

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
	StoreAssets.Reset();
	
	for(int32 i = 0; i < ShopItemsPerRefresh; i++)
	{
		const int32 RandomUpdateIndex = FMath::RandRange(1, WeaponDataAssets.Num()) - 1;
		StoreAssets.Add(WeaponDataAssets[RandomUpdateIndex]);
	}

	OnShopRefreshed.Broadcast();
}