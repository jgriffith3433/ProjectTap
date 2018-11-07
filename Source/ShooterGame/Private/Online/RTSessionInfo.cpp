// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "RTSessionInfo.h"

RTSessionInfo::RTSessionInfo(const MatchDetailsResponse& message)
{
	PortID = FString::FromInt(message.GetPort().GetValueOrDefault(-1));
	HostURL = FString(UTF8_TO_TCHAR(message.GetHost().GetValueOrDefault("").c_str()));
	AccessToken = FString(UTF8_TO_TCHAR(message.GetAccessToken().GetValueOrDefault("").c_str()));
	MatchID = FString(UTF8_TO_TCHAR(message.GetMatchId().GetValueOrDefault("").c_str()));
	PeerId = message.GetPeerId().GetValueOrDefault(-1);
	PlayerId = FString(UTF8_TO_TCHAR(message.GetPlayerId().GetValueOrDefault("").c_str()));

	for (std::size_t i = 0; i < message.GetOpponents().size(); i++)
	{
		RTPlayer player = RTPlayer(message.GetOpponents()[i]);
		PlayerList.Add(player);
	}
}

RTSessionInfo::~RTSessionInfo()
{
}
