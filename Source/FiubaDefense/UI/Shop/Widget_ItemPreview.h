// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_ItemPreview.generated.h"

class UImage;
class UTextBlock;
class UFDWeaponDataAsset;

struct FDShopItem;

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_ItemPreview : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	void DisplayPreview(TWeakPtr<FDShopItem> Item);

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_WeaponName;
};
