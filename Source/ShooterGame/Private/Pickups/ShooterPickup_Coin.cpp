// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPickup_Coin.h"



AShooterPickup_Coin::AShooterPickup_Coin(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RespawnTime = 0.0f;
	CoinAmount = 50;
}

bool AShooterPickup_Coin::CanBePickedUp(class AShooterCharacter* TestPawn) const
{
	return TestPawn != NULL;
}

void AShooterPickup_Coin::GivePickupTo(class AShooterCharacter* Pawn)
{
	if (Pawn)
	{
		Pawn->PickedUpCoinAmount = CoinAmount;

		// Fire event for collected coin
		const auto Events = Online::GetEventsInterface();
		const auto Identity = Online::GetIdentityInterface();

		if (Events.IsValid() && Identity.IsValid())
		{
			AShooterPlayerController* PC = Cast<AShooterPlayerController>(Pawn->Controller);
			if (PC)
			{
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);

				if (LocalPlayer)
				{
					const int32 UserIndex = LocalPlayer->GetControllerId();
					TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
					if (UniqueID.IsValid())
					{
						FVector Location = Pawn->GetActorLocation();

						FOnlineEventParms Params;

						Params.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
						Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
						Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

						Params.Add(TEXT("ItemId"), FVariantData((int32)0)); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
						Params.Add(TEXT("AcquisitionMethodId"), FVariantData((int32)0)); // unused
						Params.Add(TEXT("LocationX"), FVariantData(Location.X));
						Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
						Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));
						Params.Add(TEXT("ItemQty"), FVariantData(CoinAmount));

						Events->TriggerEvent(*UniqueID, TEXT("CollectPowerup"), Params);
					}
				}
			}
		}
	}
}
