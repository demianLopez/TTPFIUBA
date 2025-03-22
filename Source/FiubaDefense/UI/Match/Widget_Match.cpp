// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Match/Widget_Match.h"

#include "Components/Button.h"
#include "Game/FDGameMode.h"
#include "UI/Shop/Widget_Shop.h"
#include "Input/CommonUIInputTypes.h"
#include "UI/Widget_HUD.h"

void UWidget_Match::RegisterInputActions()
{
	Super::RegisterInputActions();
	
	{
		FBindUIActionArgs InputActionArgs(InputActionOpenShop, false, FSimpleDelegate::CreateUObject(this, &ThisClass::OpenShop));
		InputActionArgs.InputMode = ECommonInputMode::Game;	
		RegisterUIActionBinding(InputActionArgs);
	}
}

void UWidget_Match::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_NextTurnButton->OnClicked.AddDynamic(this, &ThisClass::OnNextTurnClicked);
}

void UWidget_Match::OnNextTurnClicked()
{
	AFDGameMode* GameMode = GetWorld()->GetAuthGameMode<AFDGameMode>();
	if (IsValid(GameMode))
	{
		GameMode->AdvanceTurn();
	}
}

void UWidget_Match::OpenShop()
{
	UWidget_HUD* HUD = GetHUDWidget();
	HUD->PushWidget(ShopWidget, EFD_Game);
}
