// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Props/ShooterProp.h"
#include "ShooterProp_Explosive.generated.h"

class USphereComponent;
class UMaterialInterface;
class UParticleSystem;

UCLASS()
class SHOOTERGAME_API AShooterProp_Explosive : public AShooterProp
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Prop)
		float ExplosionRadius;
	UPROPERTY(EditDefaultsOnly, Category = Prop)
		float ExplosionDamage;
	UPROPERTY(EditDefaultsOnly, Category = Prop)
		float PropImpulseMagnitude;
	UPROPERTY(EditDefaultsOnly, Category = Prop)
		UParticleSystem* ExplosionParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category = Prop)
		UMaterialInterface* ExplosionMaterial;

	virtual void PostInitializeComponents() override;
	virtual void PlayDeath(float Damage, FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	UFUNCTION()
	void OnRep_Exploded();
};
