// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Enemies/ShooterEnemyCharacter.h"
#include "Bots/ShooterAIController.h"

AShooterEnemyCharacter::AShooterEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = AShooterEnemyAIController::StaticClass();

	UpdatePawnMeshes();

	bUseControllerRotationYaw = true;
}

void AShooterEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* const World = GetWorld();
	AShooterGameState* const GameState = World ? World->GetGameState<AShooterGameState>() : nullptr;

	if (GameState)
	{
		GameState->NumEnemiesSpawnedThisWave++;
		GameState->NumEnemiesAlive++;
	}
}

bool AShooterEnemyCharacter::IsFirstPerson() const
{
	return false;
}

void AShooterEnemyCharacter::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}
