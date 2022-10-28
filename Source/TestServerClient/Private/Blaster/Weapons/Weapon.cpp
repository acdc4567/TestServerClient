// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/Weapons/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Player/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Blaster/Weapons/Casing.h"
#include "Engine/SkeletalMeshSocket.h"






AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = 0;

	bReplicates = 1;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(RootComponent);




}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();


	if (HasAuthority()) {
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);

		

	}

	if (PickupWidget) {

		PickupWidget->SetVisibility(0);


	}


	
}

void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);



}

void AWeapon::Fire(const FVector& HitTarget) {
	if (!WeaponMesh)return;
	if (FireAnimation) {
		WeaponMesh->PlayAnimation(FireAnimation, 0);
	}
	if (CasingClass) {
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName("AmmoEject");
		if (AmmoEjectSocket) {

			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			
			UWorld* World = GetWorld();
			if (World) {
				World->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
			}
			

		}
	}


}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter) {

		BlasterCharacter->SetOverlappingWeapon(this);


	}

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter) {

		BlasterCharacter->SetOverlappingWeapon(nullptr);

	}
}



void AWeapon::SetWeaponState(EWeaponState ToState) {

	WeaponState = ToState;


	switch (WeaponState) {
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(0);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}


	

}




void AWeapon::OnRep_WeaponState() {
	switch (WeaponState) {
	case EWeaponState::EWS_Initial:

		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(0);
		
		break;
	case EWeaponState::EWS_Dropped:

		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}


}




void AWeapon::ShowPickupWidget(bool bShowWidget) {

	if (PickupWidget) {
		PickupWidget->SetVisibility(bShowWidget);
	}

}



