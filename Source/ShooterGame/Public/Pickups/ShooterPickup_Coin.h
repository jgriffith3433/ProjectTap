// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/ShooterPickup.h"
#include "ShooterPickup_Coin.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterPickup_Coin : public AShooterPickup
{
	GENERATED_UCLASS_BODY()
public:
	/** check if pawn can use this pickup */
	virtual bool CanBePickedUp(AShooterCharacter* TestPawn) const override;

protected:

	/** how many coins does it give? */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
		int32 CoinAmount;

	/** give pickup */
	virtual void GivePickupTo(AShooterCharacter* Pawn) override;
	
};
