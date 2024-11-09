// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_HUD.generated.h"

class UWidget_FDActivatable;
class UCommonActivatableWidgetStack;
class UCommonActivatableWidgetContainerBase;

UENUM()
enum EFDLayer
{
	EFD_Game,
	EFD_GameNotification,
	EFD_Menu,
	EFD_PopUp
};

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UWidget_HUD : public UCommonUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	UWidget_FDActivatable* PushWidget(TSubclassOf<UWidget_FDActivatable> WidgetClass, EFDLayer Layer);
	UWidget_FDActivatable* PushWidget(TSubclassOf<UWidget_FDActivatable> WidgetClass, EFDLayer Layer, TFunctionRef<void(UWidget_FDActivatable*)> InstanceInitFunc);

	
	template<class T>
	T* PushWidget(TSubclassOf<UWidget_FDActivatable> WidgetClass, EFDLayer Layer)
	{
		return Cast<T>(PushWidget(WidgetClass, Layer));
	}
	
	UFUNCTION(BlueprintCallable)
	void RemoveWidget(UWidget_FDActivatable* Widget, EFDLayer Layer);
	
protected:

	UCommonActivatableWidgetContainerBase* GetContainerBase(EFDLayer Layer) const;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> UCommonActivatable_MenuStack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> UCommonActivatable_GameStack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> UCommonActivatable_GameNotificationStack;	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> UCommonActivatable_PopUpStack;
};
