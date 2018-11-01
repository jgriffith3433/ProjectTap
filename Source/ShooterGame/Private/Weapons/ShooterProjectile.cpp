// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Weapons/ShooterProjectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Effects/ShooterExplosionEffect.h"
#include "Player/ShooterCharacter.h"

AShooterProjectile::AShooterProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	// TODO: Find a way to ignore only owner pawn, for now we ignore all pawns
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootComponent = CollisionComp;

	ParticleComp = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = false;
	ParticleComp->bAutoDestroy = false;
	ParticleComp->SetupAttachment(RootComponent);

	MovementComp = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;

	bExplodeOnImpact = true;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}

void AShooterProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MovementComp->ProjectileGravityScale = ProjectileGravityScale;
	// Note: the MoveIgnoreActors and IgnoreActorWhenMoving functions only work if the collision comp isn't simulating physics. what's with that?
	CollisionComp->MoveIgnoreActors.Add(Instigator);
	CollisionComp->IgnoreActorWhenMoving(Instigator, true);
	
	AShooterWeapon_Projectile* OwnerWeapon = Cast<AShooterWeapon_Projectile>(GetOwner());
	if (OwnerWeapon)
	{
		OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
	}

	if (bExplodeOnDestroy)
	{
		FTimerHandle TimerHandle_Explode;
		GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &AShooterProjectile::ExplodeThenDie, WeaponConfig.ProjectileLife);
	}
	if (bExplodeOnImpact)
	{
		SetLifeSpan(WeaponConfig.ProjectileLife);
		MovementComp->OnProjectileStop.AddDynamic(this, &AShooterProjectile::OnImpact);
	}
	MyController = GetInstigatorController();
	ProjectileSpawnLocation = GetActorLocation();
}

void AShooterProjectile::InitVelocity(FVector& ShootDire)
{
	ShootDirection = ShootDire;
	if (MovementComp)
	{
		MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
	}
}

void AShooterProjectile::OnImpact(const FHitResult& HitResult)
{
	if (Role == ROLE_Authority && !bExploded)
	{
		Explode(HitResult);
		DisableAndDestroy();
	}
}

void AShooterProjectile::ExplodeThenDie()
{
	if (Role == ROLE_Authority && !bExploded)
	{
		FHitResult Impact;

		Impact.ImpactPoint = GetActorLocation();
		Impact.ImpactNormal = FVector::UpVector;

		Explode(Impact);
		DisableAndDestroy();
	}
}

void AShooterProjectile::Explode(const FHitResult& Impact)
{
	if (ParticleComp)
	{
		ParticleComp->Deactivate();
	}

	// effects and damage origin shouldn't be placed inside mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
	const FVector NudgedImpactLocationForward = Impact.ImpactPoint + ShootDirection * 100.0f;

	if (WeaponConfig.ExplosionRadius > 0 && WeaponConfig.DamageType)
	{
		AActor* ImpactActor = Impact.GetActor();
		TSubclassOf<UDamageType> const ValidDamageTypeClass = WeaponConfig.DamageType ? WeaponConfig.DamageType : TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FPointDamageEvent DmgEvent;

		DmgEvent.DamageTypeClass = ValidDamageTypeClass;
		DmgEvent.HitInfo = Impact;
		DmgEvent.Damage = WeaponConfig.ExplosionDamage;
		DmgEvent.ShotDirection = ShootDirection;

		TArray<AActor*> IgnoreActors = TArray<AActor*>();
		if (ImpactActor)
		{
			if (ImpactActor->IsA(AShooterCharacter::StaticClass()))
			{
				AShooterCharacter* ShooterChar = Cast<AShooterCharacter>(ImpactActor);
				UShooterCharacterMovement* CharacterMovementComp = Cast<UShooterCharacterMovement>(ShooterChar->GetMovementComponent());
				// make the pawn fall and apply some inverse gravity velocity
				CharacterMovementComp->SetCatchAir(true);
			}

			// apply point damage to impact actor
			ImpactActor->TakeDamage(WeaponConfig.ExplosionDamage, DmgEvent, MyController.Get(), this);
			// ignore this actor when applying radial damage
			IgnoreActors.Add(ImpactActor);
		}
		// apply radial damage to other actors
		ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, Impact.ImpactPoint, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, IgnoreActors, this, MyController.Get(), true, COLLISION_WEAPON);
	}

	if (ExplosionTemplate)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
		AShooterExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AShooterExplosionEffect>(ExplosionTemplate, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

	bExploded = true;
}

static bool ComponentIsDamageableFrom2(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	FCollisionQueryParams LineParams(SCENE_QUERY_STAT(ComponentIsVisibleFrom), true, IgnoredActor);
	LineParams.AddIgnoredActors(IgnoreActors);
	FCollisionResponseParams LineResponseParams(ECR_Block);

	// Do a trace from origin to middle of box?
	UWorld* const World = VictimComp->GetWorld();
	check(World);

	FVector TraceStart = Origin;
	FVector const TraceEnd = VictimComp->Bounds.Origin;
	if (Origin == TraceEnd)
	{
		// tiny nudge so LineTraceSingle doesn't early out with no hits
		TraceStart.Z += 0.01f;
	}

	bool const bHadBlockingHit = World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams, LineResponseParams);
	if (bHadBlockingHit)
	{
		if (OutHitResult.Component == VictimComp)
		{
			return true;
		}
	}
	return false;
}

void AShooterProjectile::ApplyRadialDamage(const UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AController* InstigatedByController, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{
	float DamageFalloff = bDoFullDamage ? 0.f : 1.f;
	ApplyRadialDamageWithFalloff(WorldContextObject, BaseDamage, 0.f, Origin, 0.f, DamageRadius, DamageFalloff, DamageTypeClass, IgnoreActors, DamageCauser, InstigatedByController, DamagePreventionChannel);
}

void AShooterProjectile::ApplyRadialDamageWithFalloff(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AController* InstigatedByController, ECollisionChannel DamagePreventionChannel)
{
	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(ApplyRadialDamage), false, DamageCauser);
		SphereParams.AddIgnoredActors(IgnoreActors);

		World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, DamagePreventionChannel, FCollisionShape::MakeSphere(DamageOuterRadius), SphereParams);
		//World->ComponentOverlapMultiByChannel(Overlaps, SphereComp, Origin, FQuat::Identity, DamagePreventionChannel);
	}

	if (Overlaps.Num() > 0)
	{
		// make sure we have a good damage type
		TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());

		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = ValidDamageTypeClass;
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);
		DmgEvent.ComponentHits = TArray<FHitResult>();
		TMap<AActor*, TArray<FHitResult> > DamagableComponentMap;

		for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
		{
			FOverlapResult const& Overlap = Overlaps[Idx];
			AActor* const OverlapActor = Overlap.GetActor();
			UPrimitiveComponent* const OverlapComp = Overlap.GetComponent();
			if (IsValid(OverlapActor) && IsValid(OverlapComp))
			{
				FHitResult DamageFromHit;
				if (ComponentIsDamageableFrom2(OverlapComp, Origin, DamageCauser, IgnoreActors, DamagePreventionChannel, DamageFromHit))
				{
					TArray<FHitResult>& HitList = DamagableComponentMap.FindOrAdd(OverlapActor);
					HitList.Add(DamageFromHit);
				}
			}
		}
		for (auto& ActorElement : DamagableComponentMap)
		{
			AActor* Actor = ActorElement.Key;
			TArray<FHitResult>&	HitResults = ActorElement.Value;
			if (Actor && Actor->bCanBeDamaged && Actor != DamageCauser && HitResults[0].Component.IsValid())
			{
				int32 cIndex = DmgEvent.ComponentHits.Add(HitResults[0]);
				if (Actor->IsA(AShooterCharacter::StaticClass()))
				{
					AShooterCharacter* ShooterChar = Cast<AShooterCharacter>(Actor);
					UShooterCharacterMovement* CharacterMovementComp = Cast<UShooterCharacterMovement>(ShooterChar->GetMovementComponent());
					CharacterMovementComp->SetCatchAir(true);
				}
				Actor->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);
				DmgEvent.ComponentHits.RemoveAt(cIndex);
			}
		}
	}

	/*FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(ApplyRadialDamage), false, DamageCauser);

	SphereParams.AddIgnoredActors(IgnoreActors);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, DamagePreventionChannel, FCollisionShape::MakeSphere(DamageOuterRadius), SphereParams);
	}

	// collate into per-actor list of hit components
	TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
	for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
	{
		FOverlapResult const& Overlap = Overlaps[Idx];
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor &&
			OverlapActor->bCanBeDamaged &&
			OverlapActor != DamageCauser &&
			Overlap.Component.IsValid())
		{
			FHitResult Hit;
			if (DamagePreventionChannel == ECC_MAX || ComponentIsDamageableFrom2(Overlap.Component.Get(), Origin, DamageCauser, IgnoreActors, DamagePreventionChannel, Hit))
			{
				TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
		}
	}

	bool bAppliedDamage = false;

	if (OverlapComponentMap.Num() > 0)
	{
		// make sure we have a good damage type
		TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());

		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = ValidDamageTypeClass;
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);

		// call damage function on each affected actors
		for (TMap<AActor*, TArray<FHitResult> >::TIterator It(OverlapComponentMap); It; ++It)
		{
			AActor* const Victim = It.Key();
			TArray<FHitResult> const& ComponentHits = It.Value();
			DmgEvent.ComponentHits = ComponentHits;

			Victim->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);

			bAppliedDamage = true;
		}
	}*/
}

void AShooterProjectile::DisableAndDestroy()
{
	UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
	if (ProjAudioComp && ProjAudioComp->IsPlaying())
	{
		ProjAudioComp->FadeOut(0.1f, 0.f);
	}

	MovementComp->StopMovementImmediately();

	// give clients some time to show explosion
	SetLifeSpan(2.0f);
}

///CODE_SNIPPET_START: AActor::GetActorLocation AActor::GetActorRotation
void AShooterProjectile::OnRep_Exploded()
{
	FHitResult Impact;
	Impact.ImpactPoint = GetActorLocation();
	if (bExplodeOnDestroy)
	{
		Impact.ImpactNormal = FVector::UpVector;
	}
	if (bExplodeOnImpact)
	{
		FVector ProjDirection = GetActorForwardVector();
		const FVector StartTrace = GetActorLocation() - ProjDirection * 200;
		const FVector EndTrace = GetActorLocation() + ProjDirection * 150;
		if (!GetWorld()->LineTraceSingleByChannel(Impact, StartTrace, EndTrace, COLLISION_PROJECTILE, FCollisionQueryParams(SCENE_QUERY_STAT(ProjClient), true, Instigator)))
		{
			// failsafe
			Impact.ImpactNormal = -ProjDirection;
		}
	}

	Explode(Impact);
}
///CODE_SNIPPET_END

void AShooterProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MovementComp)
	{
		MovementComp->Velocity = NewVelocity;
	}
}

void AShooterProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterProjectile, bExploded);
}