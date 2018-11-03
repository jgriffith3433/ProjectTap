// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "RTSessionInfo.h"

RTSessionInfo::RTSessionInfo(MatchFoundMessage message)
{
	PortID = FString::FromInt(message.GetPort().GetValueOrDefault(-1));
	HostURL = FString(UTF8_TO_TCHAR(message.GetHost().GetValueOrDefault("").c_str()));
	AccessToken = FString(UTF8_TO_TCHAR(message.GetAccessToken().GetValueOrDefault("").c_str()));
	MatchID = FString(UTF8_TO_TCHAR(message.GetMatchId().GetValueOrDefault("").c_str()));

	for (std::vector<Participant>::iterator it = message.GetParticipants().begin(); it != message.GetParticipants().end(); ++it)
	{
		PlayerList.push_back(RTPlayer(*it));
	}
}

RTSessionInfo::~RTSessionInfo()
{
}
