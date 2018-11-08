// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/ShooterPlayerController_Menu.h"
#include "ChatInputProcessor.h"

FChatInputProcessor::FChatInputProcessor(AShooterPlayerController_Menu* PC)
{
	PlayerController = PC;
}

void FChatInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{

}

bool FChatInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (PlayerController)
	{
		if (InKeyEvent.GetKey() == EKeys::Enter)
		{
			if (PlayerController->bChatCanHandleEnter)
			{
				return PlayerController->OnEnterDown();
			}
		}
		else if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			if (PlayerController->bChatCanHandleEscape)
			{
				return PlayerController->OnEscapeDown();
			}
		}
		else if (InKeyEvent.GetKey() == EKeys::Tab)
		{
			if (PlayerController->bChatCanHandleTab)
			{
				return PlayerController->OnTabDown();
			}
		}
	}
	return false;
}

bool FChatInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
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
	PlayerController = nullptr;
}