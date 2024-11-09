// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FDHUD.h"
#include "Widget_HUD.h"
#include "Blueprint/UserWidget.h"

void AFDHUD::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BaseHUDWidgetClass))
	{
		BaseHUDInstance = CreateWidget<UWidget_HUD>(PlayerOwner.Get(), BaseHUDWidgetClass);
		BaseHUDInstance->AddToViewport();
	}
}
