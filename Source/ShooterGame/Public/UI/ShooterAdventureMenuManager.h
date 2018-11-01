// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterAdventureMenuManager.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERGAME_API AShooterAdventureMenuManager : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = GameSparks)
		TSubclassOf<class UUserWidget> AdventureWidgetClass;

	UFUNCTION(BlueprintCallable)
		bool PlayMission(FString LocationName, FString MissionName);

	UFUNCTION(BlueprintImplementableEvent)
		void ShowAdventureScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideAdventureScreen();
};
