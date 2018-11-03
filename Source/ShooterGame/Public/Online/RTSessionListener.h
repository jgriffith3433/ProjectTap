// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameSparksRT/IRTSessionListener.hpp>

using namespace GameSparks::RT;

class UShooterGameInstance;

/**
 * 
 */
class SHOOTERGAME_API RTSessionListener : public IRTSessionListener
{
public:
	RTSessionListener(UShooterGameInstance* GameInstance);
	UShooterGameInstance* GameInstance;
	void OnPlayerConnect(int peerId) override;
	void OnPlayerDisconnect(int peerId) override;
	void OnReady(bool ready) override;
	void OnPacket(const GameSparks::RT::RTPacket& packet) override;
	~RTSessionListener();
};
