// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "RTSessionListener.h"

RTSessionListener::RTSessionListener(UShooterGameInstance* _GameInstance)
{
	GameInstance = _GameInstance;
}

void RTSessionListener::OnPlayerConnect(int peerId)
{
	UE_LOG(LogOnlineGame, Log, TEXT("GSM| OnPlayerConnect"));
	GameInstance->OnPlayerConnect(peerId);
}

void RTSessionListener::OnPlayerDisconnect(int peerId)
{
	UE_LOG(LogOnlineGame, Log, TEXT("GSM| OnPlayerDisconnect"));
	GameInstance->OnPlayerDisconnect(peerId);
}

void RTSessionListener::OnReady(bool ready)
{
	UE_LOG(LogOnlineGame, Log, TEXT("GSM| OnReady"));
	if (ready)
	{
		GameInstance->OnJoinRTSession(FString("/Game/Maps/AlienLab/Lab_Deathmatch"));
	}
}

void RTSessionListener::OnPacket(const RTPacket& packet)
{
	UE_LOG(LogOnlineGame, Log, TEXT("GSM| OnPacket"));
	GameInstance->OnPacket(packet);
}

RTSessionListener::~RTSessionListener()
{
	GameInstance = nullptr;
}
