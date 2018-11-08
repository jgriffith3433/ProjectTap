// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Player/ShooterPlayerController_Menu.h"
#include "ShooterStyle.h"


AShooterPlayerController_Menu::AShooterPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bChatCanHandleEnter = false;
	bChatCanHandleEscape = false;
	bChatCanHandleTab = false;
}

void AShooterPlayerController_Menu::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	ChatInput = MakeShareable(new FChatInputProcessor(this));
	FSlateApplication::Get().RegisterInputPreProcessor(ChatInput);
	FShooterStyle::Initialize();
}

void AShooterPlayerController_Menu::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (ChatInput.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(ChatInput);
		ChatInput.Reset();
	}
	Super::EndPlay(EndPlayReason);
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

void AShooterPlayerController_Menu::HostQuickDeathmatch()
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->HostQuickDeathmatch();
	}
}

void AShooterPlayerController_Menu::JoinQuickDeathmatch()
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->JoinQuickDeathmatch();
	}
}

void AShooterPlayerController_Menu::StartMatch()
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->StartMatch();
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

void AShooterPlayerController_Menu::SendChallengeChatMessage(FString ChatMessage)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SendChallengeChatMessage(TCHAR_TO_UTF8(*ChatMessage));
	}
}

void AShooterPlayerController_Menu::SendTeamChatMessage(FString ChatMessage)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SendTeamChatMessage(TCHAR_TO_UTF8(*ChatMessage));
	}
}