#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCppCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UEnhancedInputComponent;

UCLASS()
class AAAA_API AMyCppCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCppCharacter();
    virtual float TakeDamage(float InDamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void Move(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void Look(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void PerformAttack();

    UFUNCTION()
    void OnTakeDamage(float InDamageAmount, AController* EventInstigator, AActor* DamageCauser);
    void Die();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* AttackAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;
};