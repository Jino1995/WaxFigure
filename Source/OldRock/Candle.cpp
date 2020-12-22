// Fill out your copyright notice in the Description page of Project Settings.


#include "Candle.h"

// Sets default values
ACandle::ACandle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Mesh(TEXT("/Game/Annabel/SM_MLP_01_Small_Candelabre_02.SM_MLP_01_Small_Candelabre_02"));
	Mesh->SetStaticMesh(SK_Mesh.Object); 

	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY2"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Mesh2(TEXT("/Game/Annabel/SM_MLP_01_Candle_05.SM_MLP_01_Candle_05"));
	Mesh2->SetStaticMesh(SK_Mesh2.Object);
	Fire = CreateDefaultSubobject<UParticleSystem>(TEXT("FIRE"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_Fire(TEXT("/Game/Annabel/particles/P_Fire_MOBILE.P_Fire_MOBILE"));
	Fire = PS_Fire.Object;
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("POINTLIGHT"));
}
// Called when the game starts or when spawned
void ACandle::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::SpawnEmitterAttached(Fire, Mesh2, TEXT("FireSocket"), FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.1f, 0.1f, 0.05f));
	
	PointLight->AttachToComponent(Mesh2, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("FireSocket"));
	Mesh2->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CandleSocket"));
}
void ACandle::FireSize(float size) {
	PointLight->SetAttenuationRadius(size);
}
void ACandle::CandleSize(float size) {
	Torchsize = FVector(2.f, 2.f, 5.f*(size / InitSize));
	Mesh2->SetRelativeScale3D(Torchsize);
}
void ACandle::FireInitSize(float size)
{
	InitSize = size;
}
