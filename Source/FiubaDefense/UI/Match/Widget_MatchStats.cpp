// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Match/Widget_MatchStats.h"

#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "Player/FDPlayerState.h"

void UWidget_MatchStats::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	AFDPlayerState* OwningPlayerState = GetOwningPlayerState<AFDPlayerState>();

	if(IsValid(OwningPlayerState))
	{
		FNumberFormattingOptions FormattingOptions;
		FormattingOptions.MaximumFractionalDigits = 0;
		FormattingOptions.MinimumFractionalDigits = 0;
		
		TextBlock_PlayerGold->SetText(FText::AsNumber(OwningPlayerState->GetGold(), &FormattingOptions));
		TextBlock_KilledEnemies->SetText(FText::AsNumber(OwningPlayerState->GetEnemiesKilled()));
	}
}
