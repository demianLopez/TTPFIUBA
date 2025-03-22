// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_MatchStats.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_MatchStats : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_PlayerGold;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_PlayerHealth;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_KilledEnemies;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_TurnsElapsed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HealthBar;
};
