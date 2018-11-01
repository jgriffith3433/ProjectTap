// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterGameState.generated.h"

/** ranked PlayerState map, created from the GameState */
typedef TMap<int32, TWeakObjectPtr<AShooterPlayerState> > RankedPlayerMap; 

UCLASS()
class AShooterGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

public:

	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	UPROPERTY(Transient, Replicated)
	int32 NumTeams;

	/** accumulated score per team */
	UPROPERTY(Transient, Replicated)
	TArray<int32> TeamScores;

	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;

	/** current wave */
	UPROPERTY(Transient, Replicated)
		int32 CurrentWave;

	/** current wave */
	UPROPERTY(Transient, Replicated)
		int32 NumEnemiesAlive;

	/** number of enemies that have been spawned this wave */
	UPROPERTY(Transient, Replicated)
		int32 NumEnemiesSpawnedThisWave;

	/** number of enemies to spawn this wave */
	UPROPERTY(Transient, Replicated)
		int32 NumEnemiesToSpawnThisWave;

	/** 0 = preparing, 1 = fighting */
	UPROPERTY(Transient, Replicated)
		int32 WaveState;

	/** gets ranked PlayerState map for specific team */
	void GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const;	

	void RequestFinishAndExitToMainMenu();
};
