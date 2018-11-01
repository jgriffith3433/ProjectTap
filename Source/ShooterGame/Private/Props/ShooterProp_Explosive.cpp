// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Props/ShooterProp_Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AShooterProp_Explosive::AShooterProp_Explosive(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ExplosionRadius = 500.0f;
	PropImpulseMagnitude = 1000.0f;
	PropHealth = StartingPropHealth;
	bExploded = false;
	SetReplicates(true);
	SetReplicateMovement(true);
}


void AShooterProp_Explosive::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AShooterProp_Explosive::OnRep_Exploded()
{
	// for now we set replicates movement
	// if necessary in the future we can apply the impulse on the client as well so we don't have to replicate movement.
	StaticMeshComp->SetMaterial(0, ExplosionMaterial);
	UGameplayStatics::SpawnEmitterAttached(ExplosionParticleSystem, RootComponent, NAME_None, GetActorLocation());
}

void AShooterProp_Explosive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShooterProp_Explosive, bExploded);
}

static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
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


void AShooterProp_Explosive::PlayDeath(float Damage, FDamageEvent const& DamageEvent, APawn* EventInstigator, AActor* DamageCauser)
{
	Super::PlayDeath(Damage, DamageEvent, EventInstigator, DamageCauser);
	FVector Origin = GetActorLocation();
	TArray<FOverlapResult> Overlaps;
	TArray<AActor*> IgnoreActors = TArray<AActor*>();
	//IgnoreActors.Add(this);

	if (UWorld* World = GetWorld())
	{
		FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(ApplyRadialDamage), false, DamageCauser);
		SphereParams.AddIgnoredActors(IgnoreActors);

		World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, COLLISION_WEAPON, FCollisionShape::MakeSphere(ExplosionRadius), SphereParams);
	}

	if (Overlaps.Num() > 0)
	{
		// make sure we have a good damage type
		TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageType ? DamageType : TSubclassOf<UDamageType>(UDamageType::StaticClass());

		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = ValidDamageTypeClass;
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(ExplosionDamage, ExplosionDamage, ExplosionRadius, ExplosionRadius, 0.0f);
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
				if (ComponentIsDamageableFrom(OverlapComp, Origin, DamageCauser, IgnoreActors, COLLISION_WEAPON, DamageFromHit))
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
				Actor->TakeDamage(ExplosionDamage, DmgEvent, NULL, DamageCauser);
				DmgEvent.ComponentHits.RemoveAt(cIndex);
			}
		}
		FHitResult HitInfo;
		FVector ImpulseDir;

		if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
		{
			const FRadialDamageEvent* MyRadialDamageEvent = (FRadialDamageEvent*)&DamageEvent;
			MyRadialDamageEvent->GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);
		}
		else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			const FPointDamageEvent* MyPointDamageEvent = (FPointDamageEvent*)&DamageEvent;
			MyPointDamageEvent->GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);
		}
		else
		{
			DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);
		}
		ImpulseDir.Z = 1.0f;
		StaticMeshComp->AddImpulse(ImpulseDir * PropImpulseMagnitude, NAME_None, true);
		bExploded = true;
		OnRep_Exploded();
	}
}
