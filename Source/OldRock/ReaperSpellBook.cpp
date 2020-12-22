// Fill out your copyright notice in the Description page of Project Settings.


#include "ReaperSpellBook.h"

// Sets default values
AReaperSpellBook::AReaperSpellBook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DEFAULTROOT"));
	SetRootComponent(DefaultRootComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("/Game/Monsters/Boss/Stage1/TheReaper/Meshes/Characters/Separates/Acessories/SK_BookOpen.SK_BookOpen"));
	if(SK_Mesh.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_Mesh.Object);
	}

	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimIns(TEXT("/Game/Monsters/Boss/Stage1/TheReaper/AnimBP_SpellBook.AnimBP_SpellBook_C"));
	if (AnimIns.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_Mesh.Object);
	}
	Mesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AReaperSpellBook::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReaperSpellBook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

