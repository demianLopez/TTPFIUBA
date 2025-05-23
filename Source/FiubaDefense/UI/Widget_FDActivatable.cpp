// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget_FDActivatable.h"

#include "FDHUD.h"

UWidget_FDActivatable::UWidget_FDActivatable()
{
	InputMappingPriority = 10;
}

TOptional<FUIInputConfig> UWidget_FDActivatable::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case ECWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case ECWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case ECWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case ECWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

UWidget_HUD* UWidget_FDActivatable::GetHUDWidget() const
{
	AFDHUD* FDHUD = GetOwningPlayer()->GetHUD<AFDHUD>();
	if(IsValid(FDHUD))
	{
		return FDHUD->GetBaseCanvasWidget();
	}

	return nullptr;
}

void UWidget_FDActivatable::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterInputActions();
}

void UWidget_FDActivatable::RegisterInputActions()
{
}