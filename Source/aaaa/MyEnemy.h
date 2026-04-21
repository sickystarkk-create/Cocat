#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnemy.generated.h"

UCLASS()
class AAAA_API AMyEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AMyEnemy();

protected:
    virtual void BeginPlay() override;
    virtual float TakeDamage(float InDamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* VisualMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    UFUNCTION()
    void OnTakeDamage(float InDamageAmount, class AController* EventInstigator, AActor* DamageCauser);

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void Die();

    FTimerHandle FollowTimerHandle;
    void UpdateTarget();

    AActor* TargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float DamageAmount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackCooldown;

    FTimerHandle AttackTimerHandle;
    bool bCanAttack;
};