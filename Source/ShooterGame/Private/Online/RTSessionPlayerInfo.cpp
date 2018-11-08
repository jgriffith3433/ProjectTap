// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "RTPlayer.h"


RTSessionPlayerInfo::RTSessionPlayerInfo(Participant& participant)
{
	DisplayName = FString(UTF8_TO_TCHAR(participant.GetDisplayName().GetValueOrDefault("").c_str()));
	ID = FString(UTF8_TO_TCHAR(participant.GetId().GetValueOrDefault("").c_str()));
	PeerID = participant.GetPeerId().GetValueOrDefault(-1);
}

RTSessionPlayerInfo::RTSessionPlayerInfo(Player& gsPlayer)
{
	DisplayName = FString(UTF8_TO_TCHAR(gsPlayer.GetDisplayName().GetValueOrDefault("").c_str()));
	ID = FString(UTF8_TO_TCHAR(gsPlayer.GetId().GetValueOrDefault("").c_str()));
	//PeerID = gsPlayer.GetPeerId().GetValueOrDefault(-1);
}

RTSessionPlayerInfo::~RTSessionPlayerInfo()
{
}
