// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "ChatInputProcessor.h"

FChatInputProcessor::FChatInputProcessor(UShooterGameInstance* _GameInstance)
{
	GameInstance = MakeShareable(_GameInstance);
}

void FChatInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{

}

bool FChatInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		if (GameInstance.IsValid())
		{
			return GameInstance->OnEnterDown();
		}
	}
	else if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (GameInstance.IsValid())
		{
			return GameInstance->OnEscapeDown();
		}
	}

	return false;
}

bool FChatInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	// Were just going to handle the key down that way we only have to check if bChatCanHandleEnter or bChatCanHandleEscape one time
	/*if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		return true;
	}
	else if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		return true;
	}*/
	return false;
}

bool FChatInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	return false;
}

bool FChatInputProcessor::HandleMouseMoveEvent(FSlateApplication &, const FPointerEvent &)
{
	return false;
}

FChatInputProcessor::~FChatInputProcessor()
{
}