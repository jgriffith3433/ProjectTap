// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSparksManager.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERGAME_API AGameSparksManager : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = GameSparks)
		TSubclassOf<class UUserWidget> LoginWidgetClass;

	UFUNCTION(BlueprintCallable)
		void OnLoginPressed(FString UserName, FString Password);

	UFUNCTION(BlueprintImplementableEvent)
		void ShowLoginScreen();
	UFUNCTION(BlueprintImplementableEvent)
		void HideLoginScreen();
};
