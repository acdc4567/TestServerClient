// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;
class UAnimMontage;



UCLASS()
class TESTSERVERCLIENT_API ABlasterCharacter : public ACharacter,public IInteractWCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();



	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void OnRep_ReplicatedMovement() override;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();


	void AimOffset(float DeltaTime);

	void CalculateAO_Pitch();

	void SimProxiesTurn();
	

	virtual void Jump() override;
	
	void FireButtonPressed();
	void FireButtonReleased();







private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Widgets, meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing= OnRep_OverlappingWeapon)
		AWeapon* OverlappingWeapon;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, Category = Components)
		UCombatComponent* Combat;

	UFUNCTION(Server,Reliable)
		void Server_EquipButtonPresssed();

	float AO_Yaw = 0.f;
	float InterpAO_Yaw = 0.f;
	float AO_Pitch = 0.f;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace=ETurningInPlace::ETIP_NotTurning;

	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* HitReactMontage;

	void PlayHitReactMontage();


	void HideCharacterIfCameraClose();

	UPROPERTY(EditAnywhere)
		float CameraThreshold = 200.f;

	bool bShouldRotateRootBone = 0;

	float TurnThreshold = .5f;

	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;

	float ProxyYaw = 0.f;

	float TimeSinceLastMovementReplication = 0.f;

	float CalculateSpeed();

	//PlayerHealth

	UPROPERTY(EditAnywhere, Category = PlayerStats)
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing= OnRep_Health,VisibleAnywhere, Category = PlayerStats)
		float Health = 0.f;


	UFUNCTION()
		void OnRep_Health();








public:	
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	AWeapon* GetEquippedWeapon();

	void PlayFireMontage(bool bAiming);

	

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastHit();

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FVector GetHitTarget() const;

	FORCEINLINE bool GetShouldRotateRootBone() const { return bShouldRotateRootBone; }

	
};
