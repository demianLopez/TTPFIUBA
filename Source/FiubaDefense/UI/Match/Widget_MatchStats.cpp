// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Match/Widget_MatchStats.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Game/FDGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Player/FDPlayerPawn.h"
#include "Player/FDPlayerState.h"
#include "Tower/FDTower.h"

void UWidget_MatchStats::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	AFDPlayerState* OwningPlayerState = GetOwningPlayerState<AFDPlayerState>();

	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MaximumFractionalDigits = 0;
	FormattingOptions.MinimumFractionalDigits = 0;

	if(IsValid(OwningPlayerState))
	{		
		TextBlock_PlayerGold->SetText(FText::AsNumber(OwningPlayerState->GetGold(), &FormattingOptions));
		TextBlock_KilledEnemies->SetText(FText::AsNumber(OwningPlayerState->GetEnemiesKilled()));
	}

	AFDPlayerPawn* PlayerPawn = GetOwningPlayerPawn<AFDPlayerPawn>();
	if(IsValid(PlayerPawn))
	{
		AFDTower* Tower = PlayerPawn->GetTowerReference();
		if(IsValid(Tower))
		{
			float CurrentHealth = Tower->GetCurrentHealth();
			float MaxHealth = Tower->GetMaxHealth();

			const float Alpha = CurrentHealth / MaxHealth;
			ProgressBar_HealthBar->SetPercent(Alpha);
			TextBlock_PlayerHealth->SetText(FText::AsNumber(CurrentHealth, &FormattingOptions));
		}
	}

	AFDGameMode* GameMode = GetWorld()->GetAuthGameMode<AFDGameMode>();
	if(IsValid(GameMode))
	{
		const float MatchElapsedTime = GameMode->GetMatchElapsedTime();

		const float Mins = FMath::RoundToZero(MatchElapsedTime/60.0f);
		const float Seconds = MatchElapsedTime - Mins * 60.0f;

		FormattingOptions.MinimumIntegralDigits = 2;
		FormattingOptions.MaximumIntegralDigits = 2;
		
		TextBlock_MatchSecsElapsed->SetText(FText::AsNumber(Seconds, &FormattingOptions));
		TextBlock_MatchMinsElapsed->SetText(FText::AsNumber(Mins, &FormattingOptions));
	}	
}
