// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterProp.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UPawnNoiseEmitterComponent;
struct FDamageEvent;
class UDamageType;

UCLASS()
class SHOOTERGAME_API AShooterProp : public AActor
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = Prop)
		float StartingPropHealth;

	UPROPERTY(Replicated)
		float PropHealth;
	bool bIsDying;

	UPROPERTY(EditAnywhere, Category = Prop)
		UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(EditAnywhere, Category = Prop)
		UPawnNoiseEmitterComponent* NoiseEmitterComp;
	UPROPERTY(EditAnywhere, Category = Prop)
		TSubclassOf<UDamageType> DamageType;

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	/** Called on the actor right before replication occurs */
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	/**
	* Kills prop.  Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);
	/** Returns True if the prop can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual void PlayDeath(float Damage, FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser);

	/** Replicate where this prop was last hit and damaged */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
		struct FTakeHitInfo LastTakeHitInfo;

	/** sets up the replication for taking a hit */
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** play hit or death on client */
	UFUNCTION()
		void OnRep_LastTakeHitInfo();

	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);
	virtual void ApplyDamageMomentum(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);
	virtual void OnDeathDestroy();
	virtual void StopAllAnimMontages();
};
