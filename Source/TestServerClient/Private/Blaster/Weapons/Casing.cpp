// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/Weapons/Casing.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"




ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = 0;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>("CasingMesh");
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(1);
	CasingMesh->SetNotifyRigidBodyCollision(1);
	CasingMesh->SetEnableGravity(1);


}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	

	CasingMesh->AddImpulse(GetActorForwardVector() * 10.f);

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);






}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	if (ShellSound) {
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}

	SetLifeSpan(5.f);


}



