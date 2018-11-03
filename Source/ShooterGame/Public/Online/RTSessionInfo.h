// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSparks/generated/GSResponses.h"
#include "GameSparks/generated/GSMessages.h"
#include "RTPlayer.h"

using namespace GameSparks::Api::Responses;
using namespace GameSparks::Api::Messages;

/**
 * 
 */
class SHOOTERGAME_API RTSessionInfo
{
public:
	RTSessionInfo(MatchFoundMessage message);
	FString HostURL;
	FString AccessToken;
	FString PortID;
	FString MatchID;
	std::vector<RTPlayer> PlayerList;


	~RTSessionInfo();
};