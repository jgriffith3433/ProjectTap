// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterTypes.h"
#include "ShooterGameInstance.h"
#include "Online/UserProfile.h"
#include "Online/RTMatch.h"
#include "UI/ChatInputProcessor.h"
#include "ShooterPlayerController_Menu.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AShooterPlayerController_Menu : public APlayerController
{
	GENERATED_UCLASS_BODY()
public:
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadOnly, Category = User)
		UUserProfile* UserProfile;

	int PeerId;
	FString PlayerId;

	UPROPERTY(BlueprintReadOnly, Category = Online)
		TArray<URTMatch*> FoundDeathmatchesMatches;

	UPROPERTY(BlueprintReadOnly, Category = Online)
		TArray<FString> AvailableChannels;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bChatCanHandleEnter;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bChatCanHandleEscape;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bChatCanHandleTab;

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
		void OnChatMessageReceived(const FString& Channel, const FString& FromPlayerId, const FString& FromDisplayName, const FString& ChatMessage);
	UFUNCTION(BlueprintImplementableEvent)
		bool OnEnterDown();
	UFUNCTION(BlueprintImplementableEvent)
		bool OnEscapeDown();
	UFUNCTION(BlueprintImplementableEvent)
		bool OnTabDown();

	UFUNCTION(BlueprintCallable)
		bool PlayMission(const FString& MapPath);
	UFUNCTION(BlueprintCallable)
		void OnLoginPressed(FString UserName, FString Password);
	UFUNCTION(BlueprintCallable)
		void OnLogoutPressed();
	UFUNCTION(BlueprintCallable)
		void FindDeathmatches();
	UFUNCTION(BlueprintCallable)
		void HostQuickDeathmatch();
	UFUNCTION(BlueprintCallable)
		void JoinQuickDeathmatch();
	UFUNCTION(BlueprintCallable)
		void SendChallengeChatMessage(FString ChatMessage);
	UFUNCTION(BlueprintCallable)
		void SendTeamChatMessage(FString ChatMessage);

private:
	TSharedPtr<FChatInputProcessor> ChatInput;
};

