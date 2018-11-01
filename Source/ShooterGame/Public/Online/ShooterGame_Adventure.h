// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/ShooterGameMode.h"
#include "ShooterGame_Adventure.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGame_Adventure : public AShooterGameMode
{
	GENERATED_UCLASS_BODY()

public:

	/** setup team changes at player login */
	void PostLogin(APlayerController* NewPlayer) override;

	/** initialize replicated game data */
	virtual void InitGameState() override;

protected:
	virtual void HandleMatchHasStarted() override;
	virtual void DefaultTimer() override;
	virtual void StartMatch() override;

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** can players damage each other? */
	virtual bool CanDealDamage(AShooterPlayerState* DamageInstigator, AShooterPlayerState* DamagedPlayer) const override;

};
