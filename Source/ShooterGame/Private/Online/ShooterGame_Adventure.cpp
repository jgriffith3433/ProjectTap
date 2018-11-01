// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGame_Adventure.h"
#include "ShooterTeamStart.h"
#include "Enemies/ShooterEnemyAIController.h"
#include "Online/ShooterPlayerState.h"
#include "TimerManager.h"




AShooterGame_Adventure::AShooterGame_Adventure(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	bNeedsBotCreation = false;
	bDelayedStart = true;
}

void AShooterGame_Adventure::PostLogin(APlayerController* NewPlayer)
{
	AShooterPlayerState* NewPlayerState = CastChecked<AShooterPlayerState>(NewPlayer->PlayerState);
	NewPlayerState->SetTeamNum(1);

	Super::PostLogin(NewPlayer);
}

void AShooterGame_Adventure::InitGameState()
{
	Super::InitGameState();

	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = 2;
	}
}

bool AShooterGame_Adventure::CanDealDamage(AShooterPlayerState* DamageInstigator, class AShooterPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamNum() != DamageInstigator->GetTeamNum());
}

bool AShooterGame_Adventure::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
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

void AShooterGame_Adventure::DefaultTimer()
{
	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);

	if (MyGameState)
	{
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


void AShooterGame_Adventure::HandleMatchHasStarted()
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

void AShooterGame_Adventure::StartMatch()
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

	SetMatchState(MatchState::InProgress);
}

