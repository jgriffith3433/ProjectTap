// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterUIManager.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERGAME_API AShooterUIManager : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	/** User Profile */
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString AccountCoins;
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString AccountLevel;

	/** Widgets */
	UPROPERTY(EditAnywhere, Category = GameSparks)
		TSubclassOf<class UUserWidget> LoginWidgetClass;
	UPROPERTY(EditAnywhere, Category = GameSparks)
		TSubclassOf<class UUserWidget> AdventureWidgetClass;

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

	UFUNCTION(BlueprintCallable)
		bool PlayMission(FString LocationName, FString MissionName);
	UFUNCTION(BlueprintCallable)
		void OnLoginPressed(FString UserName, FString Password);
};
