// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterTypes.h"
#include "ShooterGameInstance.h"
#include "ShooterPlayerController_Menu.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AShooterPlayerController_Menu : public APlayerController
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	/** User Profile */
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString AccountCoins;
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString AccountLevel;

	UFUNCTION(BlueprintImplementableEvent)
		void ShowAdventureScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideAdventureScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowUserProfile();
	UFUNCTION(BlueprintImplementableEvent)
		void HideUserProfile();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowMainMenu();
	UFUNCTION(BlueprintImplementableEvent)
		void HideMainMenu();

	UFUNCTION(BlueprintCallable)
		bool PlayMission(FString LocationName, FString MissionName);
	UFUNCTION(BlueprintCallable)
		void OnLoginPressed(FString UserName, FString Password);
	UFUNCTION(BlueprintCallable)
		void OnLogoutPressed();
};

