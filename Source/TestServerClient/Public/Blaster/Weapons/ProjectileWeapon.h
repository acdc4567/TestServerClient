// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Weapons/Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;



/**
 * 
 */
UCLASS()
class TESTSERVERCLIENT_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	

public:

	virtual void Fire(const FVector& HitTarget) override;

	





private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;






};
