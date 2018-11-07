// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <IInputProcessor.h>

class AShooterPlayerController_Menu;
/**
 * 
 */
class SHOOTERGAME_API FChatInputProcessor : virtual public IInputProcessor
{
public:
	FChatInputProcessor(UShooterGameInstance* _GameInstance);
	TSharedPtr<UShooterGameInstance> GameInstance;
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication &, const FPointerEvent &) override;
	virtual ~FChatInputProcessor();
};
