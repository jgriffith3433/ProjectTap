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

	int PeerId;
	FString PlayerId;

	UPROPERTY(BlueprintReadOnly, Category = Online)
		TArray<URTMatch*> FoundDeathmatchesMatches;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bChatCanHandleEnter;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bChatCanHandleEscape;

	UFUNCTION(BlueprintImplementableEvent)
		void ShowLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void ShowMainMenu();
	UFUNCTION(BlueprintImplementableEvent)
		void HideMainMenu();
	UFUNCTION(BlueprintImplementableEvent)
		void OnFoundDeathmatches();
	UFUNCTION(BlueprintImplementableEvent)
		void OnChatMessageReceived(int FromPeerId, const FString& FromName, const FString& ChatMessage);
	UFUNCTION(BlueprintImplementableEvent)
		bool OnEnterDown();
	UFUNCTION(BlueprintImplementableEvent)
		bool OnEscapeDown();

	UFUNCTION(BlueprintCallable)
		bool PlayMission(const FString& MapPath);
	UFUNCTION(BlueprintCallable)
		void OnLoginPressed(FString UserName, FString Password);
	UFUNCTION(BlueprintCallable)
		void OnLogoutPressed();
	UFUNCTION(BlueprintCallable)
		void FindDeathmatches();
	UFUNCTION(BlueprintCallable)
		void FindQuickDeathmatch();
	UFUNCTION(BlueprintCallable)
		void SendChatMessage(FString ChatMessage);
	UFUNCTION(BlueprintCallable)
		void SendChatMessageToPeer(int ToPeerId, FString ChatMessage);
};

