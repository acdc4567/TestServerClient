// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SphereComponent.h"
#include "InteractInterface.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Create the Agro Sphere
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetSphereRadius(200.f);
	AgroSphere->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	AgroSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&AWeaponBase::AgroSphereOverlap);
}

void AWeaponBase::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor == nullptr) return;
	
	
	
	IInteractInterface* Interface = Cast<IInteractInterface>(OtherActor);
	if (Interface) {
		Interface->InteractPure();
		Interface->Execute_Interact(OtherActor);
	}

	

}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

