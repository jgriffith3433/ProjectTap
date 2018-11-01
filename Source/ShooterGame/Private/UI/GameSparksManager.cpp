// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "GameSparksManager.h"
#include "Online.h"


AGameSparksManager::AGameSparksManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AGameSparksManager::OnLoginPressed(FString UserName, FString Password)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	IOnlineIdentityPtr identity = OnlineSub->GetIdentityInterface();

	FOnlineAccountCredentials AccountCredentials;
	AccountCredentials.Id = UserName;
	AccountCredentials.Token = Password;
	AccountCredentials.Type = "GSCredentials";
	identity->Login(0, AccountCredentials);
}