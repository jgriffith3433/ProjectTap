// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/ShooterGameMode.h"
#include "ShooterGame_Horde.generated.h"

class AShooterEnemyAIController;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGame_Horde : public AShooterGameMode
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	TSubclassOf<APawn> EnemyPawnClass;

	/** setup team changes at player login */
	void PostLogin(APlayerController* NewPlayer) override;

	/** initialize replicated game data */
	virtual void InitGameState() override;

protected:
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual void DefaultTimer() override;
	virtual void StartMatch() override;
	void TrySpawnNewEnemy();

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** can players damage each other? */
	virtual bool CanDealDamage(AShooterPlayerState* DamageInstigator, AShooterPlayerState* DamagedPlayer) const override;


	/** number of teams */
	int32 NumTeams;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		int32 MaxAliveEnemies;
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		float TimeBetweenWaves;
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		float TimeBetweenSpawns;

private:
	AShooterEnemyAIController* CreateEnemy(int32 EnemyNum);
	void InitEnemy(AShooterEnemyAIController* AIC, int32 BotNum);
	void StartWave();
	void EndWave();
	void PrepareForNextWave();
	void CheckWaveState();

	FTimerHandle TimerHandle_StartWave;
	FTimerHandle TimerHandle_EnemySpawner;
};
