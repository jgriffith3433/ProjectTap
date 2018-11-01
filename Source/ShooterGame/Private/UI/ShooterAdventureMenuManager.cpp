// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterAdventureMenuManager.h"
#include "ShooterGameInstance.h"


AShooterAdventureMenuManager::AShooterAdventureMenuManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool AShooterAdventureMenuManager::PlayMission(FString LocationName, FString MissionName)
{
	UShooterGameInstance* GameInstance = Cast<UShooterGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		return GameInstance->LoadMissionLevel(LocationName, MissionName);
	}
	return false;
}
