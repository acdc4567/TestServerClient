// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/Player/BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapons/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Player/BlasterAnimInstance.h"






ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = 1;
	CameraBoom->SocketOffset = FVector(0.f, 75.f, 75.f);




	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = 0;

	bUseControllerRotationYaw = 0;

	GetCharacterMovement()->bOrientRotationToMovement = 1;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	Combat->SetIsReplicated(1);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = 1;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);


	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	


}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon,COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);



}




float ABlasterCharacter::CalculateSpeed() {
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();

}

void ABlasterCharacter::OnRep_Health() {



}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(0);

	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled()) {
		if (OverlappingWeapon) {
			OverlappingWeapon->ShowPickupWidget(1);

		}
	}


}





void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(1);

	}
	if (LastWeapon) {
		LastWeapon->ShowPickupWidget(0);
	}

}

bool ABlasterCharacter::IsWeaponEquipped() {
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming() {
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() {
	if (Combat == nullptr)return nullptr;
	return Combat->EquippedWeapon;


	
}

void ABlasterCharacter::PlayFireMontage(bool bAiming) {
	if (!Combat || !Combat->EquippedWeapon)return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage) {

		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? "RifleAim" : "RifleHip";
		AnimInstance->Montage_JumpToSection(SectionName);




	}



}

void ABlasterCharacter::PlayHitReactMontage() {
	if (!Combat || !Combat->EquippedWeapon)return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage) {

		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName= "React1";
		
		AnimInstance->Montage_JumpToSection(SectionName);




	}



}

FVector ABlasterCharacter::GetHitTarget() const {
	if(!Combat)return FVector();

	return Combat->HitTarget;
	
}


void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;

}


void ABlasterCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled()) {
		AimOffset(DeltaTime);
	}
	else {
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > .25f) {
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();

	}
	
	
	HideCharacterIfCameraClose();
	
	
	



}


void ABlasterCharacter::MoveForward(float Value) {
	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);

	}



}

void ABlasterCharacter::MoveRight(float Value) {

	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);

	}


}

void ABlasterCharacter::Turn(float Value) {
	AddControllerYawInput(Value * .7f);

}

void ABlasterCharacter::LookUp(float Value) {
	AddControllerPitchInput(Value * .7f);


}

void ABlasterCharacter::EquipButtonPressed() {

	if (Combat) {
		if (HasAuthority()) {
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else {
			Server_EquipButtonPresssed();
		}
	}

}

void ABlasterCharacter::CrouchButtonPressed() {
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}
	
}

void ABlasterCharacter::AimButtonPressed() {

	if (Combat) {
		Combat->SetAiming(1);

	}
}

void ABlasterCharacter::AimButtonReleased() {
	if (Combat) {
		Combat->SetAiming(0);

	}

}

void ABlasterCharacter::AimOffset(float DeltaTime) {
	if (Combat && Combat->EquippedWeapon == nullptr)return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir) {
		bShouldRotateRootBone = 1;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace==ETurningInPlace::ETIP_NotTurning) {
			InterpAO_Yaw = AO_Yaw;
		}


		bUseControllerRotationYaw = 1;

		TurnInPlace(DeltaTime);

	}
	if (Speed > 0.f || bIsInAir) {
		bShouldRotateRootBone = 0;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;

		bUseControllerRotationYaw = 1;

		TurningInPlace = ETurningInPlace::ETIP_NotTurning;



	}

	CalculateAO_Pitch();



}

void ABlasterCharacter::CalculateAO_Pitch() {
	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);

	}
}

void ABlasterCharacter::SimProxiesTurn() {

	if (!Combat || !Combat->EquippedWeapon)return;
	bShouldRotateRootBone = 0;
	float Speed = CalculateSpeed();
	if (Speed > 0.f) {
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw= UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation,ProxyRotationLastFrame).Yaw;
	if (FMath::Abs(ProxyYaw)>TurnThreshold) {
		if (ProxyYaw>TurnThreshold) {
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold) {
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else {
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

}









void ABlasterCharacter::Jump() {
	if (bIsCrouched) {

		UnCrouch();
	}
	else {
		Super::Jump();
	}







}



void ABlasterCharacter::FireButtonPressed() {
	if (Combat) {
		Combat->FireButtonPressed(1);
	}


}

void ABlasterCharacter::FireButtonReleased() {
	if (Combat) {
		Combat->FireButtonPressed(0);
	}


}



void ABlasterCharacter::TurnInPlace(float DeltaTime) {

	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Right;

	}
	else if (AO_Yaw < -90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Left;

	}
	if (TurningInPlace!=ETurningInPlace::ETIP_NotTurning) {

		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f) {
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);


		}



	}

}

void ABlasterCharacter::MulticastHit_Implementation() {


	PlayHitReactMontage();





}

void ABlasterCharacter::HideCharacterIfCameraClose() {
	if (!IsLocallyControlled()) {
		return;
	}
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold) {
		GetMesh()->SetVisibility(0);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = 1;
		}

	}
	else {
		GetMesh()->SetVisibility(1);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = 0;
		}
	}


}





void ABlasterCharacter::Server_EquipButtonPresssed_Implementation() {
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}

}




void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	check(PlayerInputComponent);
	
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ABlasterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ABlasterCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);


}

void ABlasterCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character = this;
	}


}

void ABlasterCharacter::OnRep_ReplicatedMovement() {

	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	
	TimeSinceLastMovementReplication = 0.f;
	



}





