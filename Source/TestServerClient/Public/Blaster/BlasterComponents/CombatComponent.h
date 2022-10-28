// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"

class ABlasterCharacter;
class AWeapon;
class ABlasterPlayerController;
class ABlasterHUD;






#define TRACE_LENGTH 80000.f


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTSERVERCLIENT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();


	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);





protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
		void Server_SetAiming(bool bIsAiming);

	UFUNCTION()
		void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
		void Server_Fire(const FVector_NetQuantize& TraceHitTarget );


	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Fire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);








private:

	UPROPERTY(VisibleAnywhere, Category = Players)
		ABlasterCharacter* Character;

	UPROPERTY(VisibleAnywhere, Category = Players)
		ABlasterPlayerController* Controller;

	UPROPERTY(VisibleAnywhere, Category = Players)
		ABlasterHUD* HUD;


	UPROPERTY(ReplicatedUsing= OnRep_EquippedWeapon, VisibleAnywhere, Category = Players)
		AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
		bool bAiming = 0;

	UPROPERTY(EditAnywhere, Category = Players)
		float BaseWalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = Players)
		float AimWalkSpeed = 450.f;

	bool bFireButtonPressed=0;

	//HuDAndCrosshairs
	float CrosshairVelocityFactor = 0.f;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	//AimingAndFOV

	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomedFOV=30.f;

	UPROPERTY(VisibleAnywhere, Category = Combat)
		float CurrentFOV = 0.f;



	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);


	//AutomaticFire

	FTimerHandle FireTimer;

	

	bool bCanFire = 1;

	void StartFireTimer();
	void FireTimerFinished();

	void Fire();




public:	
	
		
};
