#include "MyEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "MyCppCharacter.h"

AMyEnemy::AMyEnemy()
{
    PrimaryActorTick.bCanEverTick = false;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(SphereMesh.Object);
    }

    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    VisualMesh->SetCollisionProfileName(TEXT("NoCollision"));

    MaxHealth = 50.0f;
    CurrentHealth = MaxHealth;

    DamageAmount = 20.0f;
    AttackCooldown = 1.0f;
    bCanAttack = true;
}

void AMyEnemy::BeginPlay()
{
    Super::BeginPlay();

    TargetActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!TargetActor) return;

    GetWorldTimerManager().SetTimer(FollowTimerHandle, this, &AMyEnemy::UpdateTarget, 0.5f, true);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemy::OnOverlapBegin);
}

void AMyEnemy::UpdateTarget()
{
    if (!TargetActor) return;
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->MoveToActor(TargetActor, 50.0f);
    }
}

void AMyEnemy::OnTakeDamage(float InDamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
    CurrentHealth -= InDamageAmount;
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
        FString::Printf(TEXT("Enemy Health: %.0f / %.0f"), CurrentHealth, MaxHealth));

    if (CurrentHealth <= 0.0f)
        Die();
}

void AMyEnemy::Die()
{
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetVisibility(false);
    GetWorldTimerManager().ClearTimer(FollowTimerHandle);
    SetLifeSpan(2.0f);
}

float AMyEnemy::TakeDamage(float InDamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    OnTakeDamage(InDamageAmount, EventInstigator, DamageCauser);
    return InDamageAmount;
}

void AMyEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bCanAttack) return;
    if (OtherActor && OtherActor != this)
    {
        AMyCppCharacter* Player = Cast<AMyCppCharacter>(OtherActor);
        if (Player)
        {
            Player->TakeDamage(DamageAmount, FDamageEvent(), GetController(), this);
            bCanAttack = false;
            GetWorldTimerManager().SetTimer(AttackTimerHandle, [this]() { bCanAttack = true; }, AttackCooldown, false);
        }
    }
}