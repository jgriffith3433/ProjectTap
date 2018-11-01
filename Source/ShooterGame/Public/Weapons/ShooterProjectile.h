// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ShooterWeapon_Projectile.h"
#include "ShooterProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UPrimitiveComponent;

// 
UCLASS(Abstract, Blueprintable)
class AShooterProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

		/** initial setup */
		virtual void PostInitializeComponents() override;

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** handle hit */
	UFUNCTION()
		void OnImpact(const FHitResult& HitResult);

	void ApplyRadialDamage(const UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AController* InstigatedByController, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel);
	void ApplyRadialDamageWithFalloff(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AController* InstigatedByController, ECollisionChannel DamagePreventionChannel);

private:
	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UParticleSystemComponent* ParticleComp;
protected:

	FVector ShootDirection;
	FVector ProjectileSpawnLocation;

	UPROPERTY(EditDefaultsOnly)
		float ProjectileGravityScale;

	UPROPERTY(EditDefaultsOnly)
		bool bExplodeOnDestroy;

	UPROPERTY(EditDefaultsOnly)
		bool bExplodeOnImpact;

	/** effects for explosion */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TSubclassOf<class AShooterExplosionEffect> ExplosionTemplate;

	/** controller that fired me (cache for damage calculations) */
	TWeakObjectPtr<AController> MyController;

	/** projectile data */
	struct FProjectileWeaponData WeaponConfig;

	/** did it explode? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Exploded)
		bool bExploded;

	/** [client] explosion happened */
	UFUNCTION()
		void OnRep_Exploded();

	/** trigger explosion */
	void Explode(const FHitResult& Impact);

	/** explode after timer */
	UFUNCTION()
		void ExplodeThenDie();

	/** shutdown projectile and prepare for destruction */
	void DisableAndDestroy();

	/** update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

protected:
	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }
};
