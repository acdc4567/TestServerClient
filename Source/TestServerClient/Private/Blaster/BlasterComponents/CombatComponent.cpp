// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Player/BlasterCharacter.h"
#include "Blaster/Weapons/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "TimerManager.h"






UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);


}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera()) {
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;

		}


	}
	
	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Character && Character->IsLocallyControlled()) {
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHUDCrosshairs(DeltaTime);

		InterpFOV(DeltaTime);

	}
	


}







void UCombatComponent::FireButtonPressed(bool bPressed) {
	bFireButtonPressed = bPressed;
	
	if (bFireButtonPressed) {
		
		Fire();
		
	}




}



void UCombatComponent::Fire() {
	if (bCanFire) {
		Server_Fire(HitTarget);
		if (EquippedWeapon) {
			bCanFire = 0;

			StartFireTimer();
		}

	}


}

void UCombatComponent::StartFireTimer() {
	if (!EquippedWeapon || !Character)return;

	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);


}

void UCombatComponent::FireTimerFinished() {
	if (!EquippedWeapon)return;
	bCanFire = 1;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic) {
		Fire();
	}

}



void UCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTarget) {
	Multicast_Fire(TraceHitTarget);



}



void UCombatComponent::Multicast_Fire_Implementation(const FVector_NetQuantize& TraceHitTarget) {


	if (!EquippedWeapon)return;
	if (Character) {
		Character->PlayFireMontage(bAiming);
		
		EquippedWeapon->Fire(TraceHitTarget);
		
	}


}


void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult) {

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (!Character) {
		return;
	}
	const FVector ShootDirection = Character->GetFollowCamera()->GetForwardVector();
	const FVector ShootDirectionUp = Character->GetFollowCamera()->GetUpVector();
	const FVector TraceStart = Character->GetFollowCamera()->GetComponentLocation() + ShootDirection * 400.f ;
	const FVector TraceStart2 = TraceStart + ShootDirectionUp * 400.f;

	const FVector TraceEnd = TraceStart + ShootDirection * TRACE_LENGTH;
	
	if (bScreenToWorld) {
		FVector Start = CrosshairWorldPosition;

		if (Character) {

			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);




		}



		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (!TraceHitResult.bBlockingHit) {
			TraceHitResult.ImpactPoint = End;
		}

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWCrosshairsInterface>()) {
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else {
			HUDPackage.CrosshairsColor = FLinearColor::Green;
		}




	}
	

	



}








void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip) {
	if (Character==nullptr||WeaponToEquip==nullptr) {
		return;
	}
	EquippedWeapon = WeaponToEquip;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket) {
		HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = 0;
	Character->bUseControllerRotationYaw = 1;




}


void UCombatComponent::OnRep_EquippedWeapon() {
	if (EquippedWeapon && Character) {
		Character->GetCharacterMovement()->bOrientRotationToMovement = 0;
		Character->bUseControllerRotationYaw = 1;


	}


}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime) {

	if (!Character || !Character->Controller)return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller) {
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;

		if (HUD) {


			if (EquippedWeapon) {

				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;

			}
			else {

				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;

			}

			//calculateSpread
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			HUDPackage.CrosshairSpread = CrosshairVelocityFactor;

			HUD->SetHUDPackage(HUDPackage);


		}



	}





}


void UCombatComponent::InterpFOV(float DeltaTime) {

	if (!EquippedWeapon)return;

	if (bAiming) {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);

	}
	if (Character&&Character->GetFollowCamera()) {
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}




}



void UCombatComponent::SetAiming(bool bIsAiming) {
	bAiming = bIsAiming;
	Server_SetAiming(bIsAiming);
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;

	}



}

void UCombatComponent::Server_SetAiming_Implementation(bool bIsAiming) {
	bAiming = bIsAiming;
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;

	}
}















