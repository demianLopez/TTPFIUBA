// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FDPlayerPawn.generated.h"

class UInputMappingContext;
class UInputAction;
class AFDTower;
struct FInputActionValue;

UCLASS()
class FIUBADEFENSE_API AFDPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFDPlayerPawn();

	void SetTowerReference(AFDTower* Tower);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void RotateAround(const FInputActionValue& InputActionValue);
	void LookUp(const FInputActionValue& InputActionValue);
	
	UPROPERTY(Transient)
	TObjectPtr<AFDTower> TowerReference;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InputActionRotateAround;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InputActionLookUp;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
};
