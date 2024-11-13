// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FDShopComponent.generated.h"


class UFDWeaponDataAsset;
struct FStreamableHandle;

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

protected:

	TSharedPtr<FStreamableHandle> WeaponStreamableHandle;
	void OnWeaponLoadedComplete();

	UPROPERTY(EditDefaultsOnly)
	int32 ShopItemsPerRefresh = 8;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const UFDWeaponDataAsset>> WeaponDataAssets;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const UFDWeaponDataAsset>> StoreAssets;
};
