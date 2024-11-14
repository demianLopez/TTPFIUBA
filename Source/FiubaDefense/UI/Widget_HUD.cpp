// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget_HUD.h"
#include "UI/Widget_FDActivatable.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UWidget_FDActivatable* UWidget_HUD::PushWidget(TSubclassOf<UWidget_FDActivatable> WidgetClass, EFDLayer Layer)
{
	return PushWidget(WidgetClass, Layer, [](UWidget_FDActivatable*){});
}

UWidget_FDActivatable* UWidget_HUD::PushWidget(TSubclassOf<UWidget_FDActivatable> WidgetClass, EFDLayer Layer,
	TFunctionRef<void(UWidget_FDActivatable*)> InstanceInitFunc)
{
	UCommonActivatableWidgetContainerBase* LayerContainer = GetContainerBase(Layer);
	if(IsValid(LayerContainer))
	{
		return LayerContainer->AddWidget<UWidget_FDActivatable>(WidgetClass, [this, &InstanceInitFunc](UWidget_FDActivatable& Widget)
		{
			InstanceInitFunc(&Widget);
		});
	}

	return nullptr;
}

void UWidget_HUD::RemoveWidget(UWidget_FDActivatable* Widget, EFDLayer Layer)
{
	UCommonActivatableWidgetContainerBase* LayerContainer = GetContainerBase(Layer);
	if(IsValid(LayerContainer) && IsValid(Widget))
	{
		LayerContainer->RemoveWidget(*Widget);
	}
}

UCommonActivatableWidgetContainerBase* UWidget_HUD::GetContainerBase(EFDLayer Layer) const
{
	switch (Layer)
	{
	case EFD_Game:
		return UCommonActivatable_GameStack;
	case EFD_GameNotification:
		return UCommonActivatable_GameNotificationStack;
	case EFD_Menu:
		return  UCommonActivatable_MenuStack;
	case EFD_PopUp:
		return UCommonActivatable_PopUpStack;
	}

	return nullptr;
}
