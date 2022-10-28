// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class ACasing;
class UTexture2D;



UENUM(BlueprintType)
enum class EWeaponState :uint8 {
	EWS_Initial UMETA(DisplayName="InitialState"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")



};



UCLASS()
class TESTSERVERCLIENT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();


	virtual void Tick(float DeltaTime) override;

	void ShowPickupWidget(bool bShowWidget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Fire(const FVector& HitTarget);

	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = .1f;

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = 1;


protected:
	virtual void BeginPlay() override;


	UFUNCTION()
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



private:

	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
		USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, Category = WeaponProperties)
		EWeaponState WeaponState;

	UFUNCTION()
		void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
		UAnimationAsset* FireAnimation;


	UPROPERTY(EditAnywhere, Category = WeaponProperties)
		TSubclassOf<ACasing> CasingClass;


	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
		UWidgetComponent* PickupWidget;

	//ZoomedFOV while aimin

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
		float ZoomedFOV=30.f;

	UPROPERTY(EditAnywhere, Category = WeaponProperties)
		float ZoomInterpSpeed = 20.f;

	





public:	
	
	void SetWeaponState(EWeaponState ToState);

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	//Textures for WeaponCrosshairs


	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }

	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }











};
