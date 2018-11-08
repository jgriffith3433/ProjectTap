// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTMatch.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom))
class URTMatch : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Online)
		FString HostPlayerId;
	UPROPERTY(BlueprintReadOnly, Category = Online)
		FString ChallengeInstanceId;
	UPROPERTY(BlueprintReadOnly, Category = Online)
		FString MapName;
	UPROPERTY(BlueprintReadOnly, Category = Online)
		int32 PlayerCount;

};
