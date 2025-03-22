// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget_FDActivatable.h"
#include "Widget_Match.generated.h"

class UButton;
class UInputAction;
class UWidget_Shop;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_Match : public UWidget_FDActivatable
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void RegisterInputActions() override;

	UFUNCTION()
	void OnNextTurnClicked();
	
	void OpenShop();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWidget_Shop> ShopWidget;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_NextTurnButton;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputActionOpenShop;
};
