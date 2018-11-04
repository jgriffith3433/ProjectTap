// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UserProfile.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom))
class UUserProfile : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = User)
		FString DisplayName;
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString AccountCoins;
	UPROPERTY(BlueprintReadOnly, Category = User)
		FString AccountLevel;

};
