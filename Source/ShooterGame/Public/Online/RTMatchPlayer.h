// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTMatch.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom))
class URTMatchPlayer : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = Online)
		FString PlayerId;


};
