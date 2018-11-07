// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Player/ShooterPlayerController_Menu.h"
#include "ShooterStyle.h"
#include "UI/ChatInputProcessor.h"


AShooterPlayerController_Menu::AShooterPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bChatCanHandleEnter = false;
	bChatCanHandleEscape = false;
}

void AShooterPlayerController_Menu::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		TSharedPtr<FChatInputProcessor> ChatInput(new FChatInputProcessor(GameInstance));
		FSlateApplication::Get().RegisterInputPreProcessor(ChatInput);
	}
	FShooterStyle::Initialize();
}

bool AShooterPlayerController_Menu::PlayMission(const FString& MapPath)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		return GameInstance->LoadMissionLevel(MapPath);
	}
	return false;
}

void AShooterPlayerController_Menu::FindDeathmatches()
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->FindDeathmatches();
	}
}

void AShooterPlayerController_Menu::FindQuickDeathmatch()
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->FindQuickDeathmatch();
	}
}

void AShooterPlayerController_Menu::OnLoginPressed(FString UserName, FString Password)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->Login(0, UserName, Password);
	}
}

void AShooterPlayerController_Menu::OnLogoutPressed()
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->Logout(0);
	}
}

void AShooterPlayerController_Menu::SendChatMessage(FString ChatMessage)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SendServerChatMessage(TCHAR_TO_UTF8(*ChatMessage));
	}
}

void AShooterPlayerController_Menu::SendChatMessageToPeer(int ToPeerId, FString ChatMessage)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SendPrivateChatMessage(ToPeerId, TCHAR_TO_UTF8(*ChatMessage));
	}
}