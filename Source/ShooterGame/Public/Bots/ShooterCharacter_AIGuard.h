// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter_AIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIGuardState : uint8
{
	StandingStill,
	Patroling,
	Suspicious,
	Alerted
};

UCLASS()
class SHOOTERGAME_API AShooterCharacter_AIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter_AIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Components)
		UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditInstanceOnly, Category = AI)
		bool bCanPatrolArea;

	UPROPERTY(EditInstanceOnly, Category = AI, meta = (EditCondition = bPatrol))
		AActor* FirstPatrolPoint;

	UPROPERTY(EditInstanceOnly, Category = AI, meta = (EditCondition = bPatrol))
		AActor* SecondPatrolPoint;

	AActor* CurrentPatrolPoint;

	UFUNCTION()
		void OnPawnSeen(APawn* pawn);

	UFUNCTION()
		void OnNoiseHeard(APawn* inst, const FVector& Location, float Volume);

	UFUNCTION()
		void ResetRotation();

	void FightPlayer(APawn* player);
	void MoveToNextControlPoint();

	FRotator OriginalRotation;
	APawn* enemyPlayer;
	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
		EAIGuardState GuardState;

	void SetGuardState(EAIGuardState newState);

	UFUNCTION()
		void OnRep_GuardState();


	FTimerHandle TimerHandle_GiveUp;
	
};
