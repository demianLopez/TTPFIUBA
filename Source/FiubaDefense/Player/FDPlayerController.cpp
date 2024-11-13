// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FDPlayerController.h"

#include "Game/FDShopComponent.h"

AFDPlayerController::AFDPlayerController()
{
	ShopComponent = CreateDefaultSubobject<UFDShopComponent>("ShopComponent");
}
