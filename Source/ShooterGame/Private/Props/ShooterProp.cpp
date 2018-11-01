// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Props/ShooterProp.h"
#include "Weapons/ShooterDamageType.h"
#include "AudioThread.h"
#include "Components/PawnNoiseEmitterComponent.h"


AShooterProp::AShooterProp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	StaticMeshComp = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	StaticMeshComp->SetSimulatePhysics(true);
	SetRootComponent(StaticMeshComp);

	NoiseEmitterComp = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("NoiseEmitterComp"));

	StartingPropHealth = 0;
}

void AShooterProp::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PropHealth = StartingPropHealth;
}

float AShooterProp::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Modify based on game rules.
	AShooterGameMode* const Game = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (PropHealth <= 0.f)
	{
		PlayHit(Damage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
	}
	else
	{
		PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : NULL);

		if (ActualDamage > 0.f)
		{
			PropHealth -= ActualDamage;
			if (PropHealth <= 0)
			{
				Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
			}
		}
	}
	return ActualDamage;
}

bool AShooterProp::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode<AShooterGameMode>() == NULL
		|| GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}

bool AShooterProp::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	PropHealth = FMath::Min(0.0f, PropHealth);

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void AShooterProp::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}
	bIsDying = true;

	PlayDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);
	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);
	}
}

void AShooterProp::PlayDeath(float Damage, FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser)
{
	FTimerHandle TimerHandle_DeathDestroy;
	GetWorldTimerManager().SetTimer(TimerHandle_DeathDestroy, this, &AShooterProp::OnDeathDestroy, 10.0f, false);
}

void AShooterProp::OnDeathDestroy()
{
	bReplicateMovement = false;
	Destroy();
	TearOff();
	StopAllAnimMontages();
}

void AShooterProp::StopAllAnimMontages()
{
	//USkeletalMeshComponent* UseMesh = GetPawnMesh();
	//if (UseMesh && UseMesh->AnimScriptInstance)
	//{
	//	UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	//}
}

void AShooterProp::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<ACharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void AShooterProp::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}

void AShooterProp::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);
	}
	ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
}

void AShooterProp::ApplyDamageMomentum(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	UDamageType const* const DmgTypeCDO = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
	float const ImpulseScale = DmgTypeCDO->DamageImpulse;

	if ((ImpulseScale > 3.f) && (StaticMeshComp != nullptr))
	{
		FHitResult HitInfo;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, PawnInstigator, HitInfo, ImpulseDir);

		FVector Impulse = ImpulseDir * ImpulseScale;
		bool const bMassIndependentImpulse = !DmgTypeCDO->bScaleMomentumByMass;

		// limit Z momentum added if already going up faster than jump (to avoid blowing prop way up into the sky)
		{
			FVector MassScaledImpulse = Impulse;
			if (!bMassIndependentImpulse && StaticMeshComp->GetMass() > SMALL_NUMBER)
			{
				MassScaledImpulse = MassScaledImpulse / StaticMeshComp->GetMass();
			}

			/*if ((StaticMeshComp->Velocity.Z > JumpZVelocity && (MassScaledImpulse.Z > 0.f))
			{
				Impulse.Z *= 0.5f;
			}*/
		}

		StaticMeshComp->AddImpulse(Impulse, NAME_None, bMassIndependentImpulse);
	}
}

void AShooterProp::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AShooterProp, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}

void AShooterProp::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterProp, LastTakeHitInfo, COND_Custom);
	DOREPLIFETIME(AShooterProp, PropHealth);
}
