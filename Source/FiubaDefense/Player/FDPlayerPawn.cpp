// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FDPlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
AFDPlayerPawn::AFDPlayerPawn()
{


}

void AFDPlayerPawn::SetTowerReference(AFDTower* Tower)
{
	TowerReference = Tower;
}

void AFDPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if(IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(InputActionRotateAround, ETriggerEvent::Triggered, this, &ThisClass::RotateAround);
		EnhancedInputComponent->BindAction(InputActionLookUp, ETriggerEvent::Triggered, this, &ThisClass::LookUp);
	}

	if (IsValid(InputMappingContext))
	{
		UWorld* World = GetWorld();
		if (ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSystem->AddMappingContext(InputMappingContext, 1);
			}
		}
	}
}

void AFDPlayerPawn::RotateAround(const FInputActionValue& InputActionValue)
{
	const float& Value = InputActionValue.Get<FInputActionValue::Axis1D>();

	if(!FMath::IsNearlyZero(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AFDPlayerPawn::LookUp(const FInputActionValue& InputActionValue)
{
	const float& Value = InputActionValue.Get<FInputActionValue::Axis1D>();

	if(!FMath::IsNearlyZero(Value))
	{
		AddControllerPitchInput(Value);
	}
}
