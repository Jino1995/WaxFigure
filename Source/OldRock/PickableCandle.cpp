// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableCandle.h"

// Sets default values
APickableCandle::APickableCandle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Mesh(TEXT("/Game/Annabel/SM_MLP_01_Small_Candelabre_02.SM_MLP_01_Small_Candelabre_02"));
	Mesh->SetStaticMesh(SK_Mesh.Object);

	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY2"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Mesh2(TEXT("/Game/Annabel/SM_MLP_01_Candle_05.SM_MLP_01_Candle_05"));
	Mesh2->SetStaticMesh(SK_Mesh2.Object);
	Mesh2->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CandleSocket"));
}

// Called when the game starts or when spawned
void APickableCandle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableCandle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

