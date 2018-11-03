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
}

void RTSessionListener::OnPlayerDisconnect(int peerId)
{
}

void RTSessionListener::OnReady(bool ready)
{
	if (ready)
	{
		GameInstance->OnJoinRTSession(FString("/Game/Maps/AlienLab/Lab_Deathmatch"));
	}
}

void RTSessionListener::OnPacket(const RTPacket& packet)
{
}

RTSessionListener::~RTSessionListener()
{
}
