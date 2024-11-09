// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FDHUD.generated.h"

class UWidget_HUD;

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDHUD : public AHUD
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UWidget_HUD* GetBaseCanvasWidget() const { return BaseHUDInstance;}
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWidget_HUD> BaseHUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWidget_HUD> BaseHUDInstance;
};
