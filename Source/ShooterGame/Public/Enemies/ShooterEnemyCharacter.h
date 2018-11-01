// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterCharacter.h"
#include "ShooterEnemyCharacter.generated.h"

UCLASS()
class AShooterEnemyCharacter : public AShooterCharacter
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Behavior)
		class UBehaviorTree* BotBehavior;

	virtual bool IsFirstPerson() const override;
	virtual void PostInitializeComponents() override;
	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;
};
