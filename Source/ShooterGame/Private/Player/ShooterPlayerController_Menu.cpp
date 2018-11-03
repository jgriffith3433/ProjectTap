// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Player/ShooterPlayerController_Menu.h"
#include "ShooterStyle.h"


AShooterPlayerController_Menu::AShooterPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
}

void AShooterPlayerController_Menu::PostInitializeComponents() 
{
	Super::PostInitializeComponents();

	FShooterStyle::Initialize();
}

bool AShooterPlayerController_Menu::PlayMission(FString LocationName, FString MissionName)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		return GameInstance->LoadMissionLevel(LocationName, MissionName);
	}
	return false;
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