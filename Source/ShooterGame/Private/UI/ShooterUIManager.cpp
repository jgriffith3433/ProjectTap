// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterUIManager.h"

AShooterUIManager::AShooterUIManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool AShooterUIManager::PlayMission(FString LocationName, FString MissionName)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		return GameInstance->LoadMissionLevel(LocationName, MissionName);
	}
	return false;
}

void AShooterUIManager::OnLoginPressed(FString UserName, FString Password)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	IOnlineIdentityPtr identity = OnlineSub->GetIdentityInterface();

	FOnlineAccountCredentials AccountCredentials;
	AccountCredentials.Id = UserName;
	AccountCredentials.Token = Password;
	AccountCredentials.Type = "GSCredentials";
	identity->Login(0, AccountCredentials);
}