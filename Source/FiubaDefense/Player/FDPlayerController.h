// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FDPlayerController.generated.h"

class UFDShopComponent;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AFDPlayerController();
	
	FORCEINLINE UFDShopComponent* GetShopComponent() const { return ShopComponent; }
protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFDShopComponent> ShopComponent;
};
