// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_FDActivatable.generated.h"

class UWidget_HUD;

UENUM(BlueprintType)
enum class ECWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_FDActivatable : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:

	UWidget_FDActivatable();

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	
protected:

	UPROPERTY(Transient)
	TObjectPtr<UWidget_HUD> OwningHUD;

	friend class UWidget_HUD;
	
	virtual void NativeOnInitialized() override;

	virtual void RegisterInputActions();

	UPROPERTY(EditDefaultsOnly, Category = Input)
	ECWidgetInputMode InputConfig = ECWidgetInputMode::Default;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;	
};
