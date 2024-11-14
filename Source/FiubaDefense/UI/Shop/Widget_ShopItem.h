// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_ShopItem.generated.h"

class UFDWeaponDataAsset;
class UTextBlock;
class UImage;
struct FDShopItem;

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_ShopItem : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	void InitWithItem(TWeakPtr<FDShopItem> ShopItem);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPreviewDelegate, TWeakPtr<FDShopItem>);
	FOnItemPreviewDelegate OnItemPreviewDelegate;

	FOnItemPreviewDelegate OnItemClicked;
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	TWeakPtr<FDShopItem> ShopItem;

	UFUNCTION(BlueprintImplementableEvent)
	void SetAsDisabled();

	UFUNCTION(BlueprintImplementableEvent)
	void SetAsEnabled();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_WeaponName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_WeaponIcon;
};
