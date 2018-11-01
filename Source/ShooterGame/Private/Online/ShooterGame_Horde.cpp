// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGame_Horde.h"
#include "ShooterTeamStart.h"
#include "Enemies/ShooterEnemyAIController.h"
#include "Online/ShooterPlayerState.h"
#include "TimerManager.h"


AShooterGame_Horde::AShooterGame_Horde(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNeedsBotCreation = false;
	bDelayedStart = true;
	NumTeams = 2;
	MaxAliveEnemies = 100;
	TimeBetweenSpawns = .5f;
	TimeBetweenWaves = 15.0f;
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> EnemyPawnOb(TEXT("/Game/Blueprints/Pawns/EnemyPawn"));
	EnemyPawnClass = EnemyPawnOb.Class;
}

void AShooterGame_Horde::PostLogin(APlayerController* NewPlayer)
{
	AShooterPlayerState* NewPlayerState = CastChecked<AShooterPlayerState>(NewPlayer->PlayerState);
	NewPlayerState->SetTeamNum(1);

	Super::PostLogin(NewPlayer);
}

void AShooterGame_Horde::InitGameState()
{
	Super::InitGameState();

	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = NumTeams;
	}
}

bool AShooterGame_Horde::CanDealDamage(AShooterPlayerState* DamageInstigator, class AShooterPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamNum() != DamageInstigator->GetTeamNum());
}

bool AShooterGame_Horde::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		AShooterTeamStart* TeamStart = Cast<AShooterTeamStart>(SpawnPoint);
		AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNum())
		{
			return false;
		}
	}

	return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}

void AShooterGame_Horde::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

void AShooterGame_Horde::DefaultTimer()
{
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);

	if (MyGameState)
	{
		CheckWaveState();
		if (MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
		{
			MyGameState->RemainingTime--;

			if (MyGameState->RemainingTime <= 0)
			{
				if (GetMatchState() == MatchState::WaitingPostMatch)
				{
					RestartGame();
				}
				else if (GetMatchState() == MatchState::InProgress)
				{
					FinishMatch();

					// Send end round events
					for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
					{
						AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(*It);

						if (PlayerController && MyGameState)
						{
							AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>((*It)->PlayerState);
							const bool bIsWinner = IsWinner(PlayerState);

							PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
						}
					}
				}
				else if (GetMatchState() == MatchState::WaitingToStart)
				{
					StartMatch();
				}
			}
		}
	}
}

void AShooterGame_Horde::CheckWaveState()
{
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);

	if (MyGameState)
	{
		if (MyGameState->WaveState == 0)
		{
			//preparing
		}
		else if (MyGameState->WaveState == 1)
		{
			//fighting
			if (MyGameState->NumEnemiesAlive <= 0)
			{
				EndWave();
			}
		}
	}
}

UClass* AShooterGame_Horde::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AShooterEnemyAIController>())
	{
		return EnemyPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AShooterEnemyAIController* AShooterGame_Horde::CreateEnemy(int32 EnemyNum)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = nullptr;

	UWorld* World = GetWorld();
	AShooterEnemyAIController* AIC = World->SpawnActor<AShooterEnemyAIController>(SpawnInfo);
	InitEnemy(AIC, EnemyNum);
	return AIC;
}

void AShooterGame_Horde::InitEnemy(AShooterEnemyAIController* AIC, int32 EnemyNum)
{
	if (AIC)
	{
		if (AIC->PlayerState)
		{
			FString EnemyName = FString::Printf(TEXT("Enemy %d"), EnemyNum);
			AIC->PlayerState->SetPlayerName(EnemyName);
			AShooterPlayerState* NewPlayerState = CastChecked<AShooterPlayerState>(AIC->PlayerState);
			NewPlayerState->SetTeamNum(2);
		}
		RestartPlayer(AIC);
	}
}

void AShooterGame_Horde::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
	MyGameState->RemainingTime = RoundTime;

	// notify players
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AShooterPlayerController* PC = Cast<AShooterPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
}

void AShooterGame_Horde::StartMatch()
{
	if (HasMatchStarted())
	{
		// Already started
		return;
	}

	//Let the game session override the StartMatch function, in case it wants to wait for arbitration
	if (GameSession->HandleStartMatchRequest())
	{
		return;
	}

	PrepareForNextWave();
	SetMatchState(MatchState::InProgress);
}

void AShooterGame_Horde::TrySpawnNewEnemy()
{
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);

	if (MyGameState)
	{
		if (MyGameState->NumEnemiesSpawnedThisWave < MyGameState->NumEnemiesToSpawnThisWave && MyGameState->NumEnemiesAlive <= MaxAliveEnemies)
		{
			// Create any necessary AIControllers.  Hold off on Pawn creation until pawns are actually necessary or need recreating.	
			CreateEnemy(MyGameState->NumEnemiesSpawnedThisWave + 1);
		}
	}
}

void AShooterGame_Horde::StartWave()
{
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);

	if (MyGameState)
	{
		MyGameState->WaveState = 1;
	}
	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &AShooterGame_Horde::TrySpawnNewEnemy, TimeBetweenSpawns, true, 0.0f);
}

void AShooterGame_Horde::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);
	PrepareForNextWave();
}

void AShooterGame_Horde::PrepareForNextWave()
{
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);

	if (MyGameState)
	{
		MyGameState->CurrentWave++;
		MyGameState->NumEnemiesSpawnedThisWave = 0;
		MyGameState->NumEnemiesToSpawnThisWave = 200;
		MyGameState->WaveState = 0;
	}
	GetWorldTimerManager().SetTimer(TimerHandle_StartWave, this, &AShooterGame_Horde::StartWave, TimeBetweenWaves, false, 0.0f);
}
