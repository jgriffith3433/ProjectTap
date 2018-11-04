// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterTypes.h"
#include "ShooterGameInstance.h"
#include "Online/UserProfile.h"
#include "Online/RTMatch.h"
#include "ShooterPlayerController_Menu.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AShooterPlayerController_Menu : public APlayerController
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	UPROPERTY(BlueprintReadOnly, Category = User)
		UUserProfile* UserProfile;

	UPROPERTY(BlueprintReadOnly, Category = Online)
		TArray<URTMatch*> RTMatches;

	UFUNCTION(BlueprintImplementableEvent)
		void ShowAdventureScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideAdventureScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowMainMenu();
	UFUNCTION(BlueprintImplementableEvent)
		void HideMainMenu();

	UFUNCTION(BlueprintCallable)
		bool PlayMission(const FString& MapPath);
	UFUNCTION(BlueprintCallable)
		void OnLoginPressed(FString UserName, FString Password);
	UFUNCTION(BlueprintCallable)
		void OnLogoutPressed();
	UFUNCTION(BlueprintCallable)
		void PlayDeathmatch();
};

