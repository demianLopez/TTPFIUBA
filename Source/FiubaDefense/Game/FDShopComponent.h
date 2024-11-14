// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FDShopComponent.generated.h"


class UFDWeaponDataAsset;
struct FStreamableHandle;

struct FDShopItem
{
	FDShopItem(int32 Index);
	
	TWeakObjectPtr<const UFDWeaponDataAsset> WeaponData;

	bool bAvailable;
	int32 Index;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIUBADEFENSE_API UFDShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFDShopComponent();

	void RefreshShop();

	virtual void BeginPlay() override;

	DECLARE_MULTICAST_DELEGATE(FOnShopRefreshed);
	FOnShopRefreshed OnShopRefreshed;

	FORCEINLINE const TArray<TSharedPtr<FDShopItem>>& GetSellingAssets() const { return SellingItems; }

	bool TryBuyItem(TSharedPtr<FDShopItem> ShopItem);
protected:

	TSharedPtr<FStreamableHandle> WeaponStreamableHandle;
	void OnWeaponLoadedComplete();

	UPROPERTY(EditDefaultsOnly)
	int32 ShopItemsPerRefresh = 8;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const UFDWeaponDataAsset>> WeaponDataAssets;
	
	TArray<TSharedPtr<FDShopItem>> SellingItems;
};
