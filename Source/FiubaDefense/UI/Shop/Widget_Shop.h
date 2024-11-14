// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget_FDActivatable.h"
#include "Widget_Shop.generated.h"

class UTextBlock;
class UFDWeaponDataAsset;
class UUniformGridPanel;
class UFDShopComponent;
class UWidget_ShopItem;
class UInputAction;
class UWidget_ItemPreview;

struct FDShopItem;

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_Shop : public UWidget_FDActivatable
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void RegisterInputActions() override;

	void RefreshShop();
	void OnItemPreview(TWeakPtr<FDShopItem> Item);
	void OnItemClicked(TWeakPtr<FDShopItem> Item);

	TWeakObjectPtr<UFDShopComponent> CachedShopComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionCloseShop;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGridPanel_ShopItems;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_ItemPreview> WidgetItemPreview;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_PlayerGold;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWidget_ShopItem> ShopItemClass;
	
	UPROPERTY(EditDefaultsOnly)
	int32 NumberOfCols = 4;
};
