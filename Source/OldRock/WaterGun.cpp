// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterGun.h"

// Sets default values
AWaterGun::AWaterGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Mesh(TEXT("/Game/Annabel/0_scene_mesh_.0_scene_mesh_"));
	Mesh->SetStaticMesh(SK_Mesh.Object);
}

// Called when the game starts or when spawned
void AWaterGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaterGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

