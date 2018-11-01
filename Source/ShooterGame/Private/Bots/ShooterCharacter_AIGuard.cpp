// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterCharacter_AIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShooterCharacter_AIGuard::AShooterCharacter_AIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AShooterCharacter_AIGuard::OnPawnSeen);
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &AShooterCharacter_AIGuard::OnNoiseHeard);
}

// Called when the game starts or when spawned
void AShooterCharacter_AIGuard::BeginPlay()
{
	Super::BeginPlay();
	if (bCanPatrolArea)
	{
		SetGuardState(EAIGuardState::Patroling);
		MoveToNextControlPoint();
	}
	else
	{
		SetGuardState(EAIGuardState::StandingStill);
	}
	OriginalRotation = GetActorRotation();
}

void AShooterCharacter_AIGuard::OnPawnSeen(APawn* pawn)
{
	GetWorldTimerManager().ClearTimer(TimerHandle_GiveUp);
	GetWorldTimerManager().SetTimer(TimerHandle_GiveUp, this, &AShooterCharacter_AIGuard::ResetRotation, 2.0f);
	SetGuardState(EAIGuardState::Alerted);
	AController* controller = GetController();
	if (controller)
	{
		controller->StopMovement();
	}
	FightPlayer(pawn);
}

void AShooterCharacter_AIGuard::MoveToNextControlPoint()
{
	if (CurrentPatrolPoint == NULL || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

void AShooterCharacter_AIGuard::OnRep_GuardState()
{

}

void AShooterCharacter_AIGuard::SetGuardState(EAIGuardState newState)
{
	if (newState == GuardState)
	{
		return;
	}

	GuardState = newState;
	OnRep_GuardState();
}

void AShooterCharacter_AIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter_AIGuard, GuardState);
}

void AShooterCharacter_AIGuard::OnNoiseHeard(APawn* Inst, const FVector& Location, float Volume)
{
	if (GuardState != EAIGuardState::Alerted)
	{
		AController* controller = GetController();
		if (controller)
		{
			controller->StopMovement();
		}
		//rotate towards location to see if we can see the player
		FVector dir = Location - GetActorLocation();
		dir.Normalize();
		FRotator newLookAt = FRotationMatrix::MakeFromX(dir).Rotator();
		newLookAt.Pitch = 0.0f;
		newLookAt.Roll = 0.0f;
		SetGuardState(EAIGuardState::Suspicious);
		GetWorldTimerManager().ClearTimer(TimerHandle_GiveUp);
		GetWorldTimerManager().SetTimer(TimerHandle_GiveUp, this, &AShooterCharacter_AIGuard::ResetRotation, 2.0f);
		OriginalRotation = GetActorRotation();
		SetActorRotation(newLookAt);
	}
}

void AShooterCharacter_AIGuard::FightPlayer(APawn* player)
{
	enemyPlayer = player;
}

void AShooterCharacter_AIGuard::ResetRotation()
{
	if (bCanPatrolArea)
	{
		SetGuardState(EAIGuardState::Patroling);
		MoveToNextControlPoint();
	}
	else
	{
		SetGuardState(EAIGuardState::StandingStill);
	}
	SetActorRotation(OriginalRotation);
}

// Called every frame
void AShooterCharacter_AIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GuardState == EAIGuardState::Patroling)
	{
		if (CurrentPatrolPoint)
		{
			FVector distanceVector = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
			float distanceToGoal = distanceVector.Size();
			if (distanceToGoal < 50.0f)
			{
				MoveToNextControlPoint();
			}
		}
	}
}


