#include "MyCppCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AMyCppCharacter::AMyCppCharacter()
{
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
}

void AMyCppCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;

        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}

void AMyCppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;

    if (MoveAction)
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCppCharacter::Move);

    if (JumpAction)
    {
        EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCppCharacter::StartJump);
        EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCppCharacter::StopJump);
    }

    if (LookAction)
        EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCppCharacter::Look);

    if (AttackAction)
        EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCppCharacter::Attack);
}

void AMyCppCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (!Controller) return;

    const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
    const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, MovementVector.Y);
    AddMovementInput(RightDir, MovementVector.X);
}

void AMyCppCharacter::StartJump()
{
    Jump();
}

void AMyCppCharacter::StopJump()
{
    StopJumping();
}

void AMyCppCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookVector = Value.Get<FVector2D>();
    if (Controller)
    {
        AddControllerYawInput(LookVector.X);
        AddControllerPitchInput(LookVector.Y);
    }
}

void AMyCppCharacter::Attack(const FInputActionValue& Value)
{
    PerformAttack();
}

void AMyCppCharacter::PerformAttack()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    FHitResult Hit;
    if (PC->GetHitResultUnderCursor(ECC_Pawn, false, Hit))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor != this)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange,
                FString::Printf(TEXT("Hit: %s"), *HitActor->GetName()));
            HitActor->TakeDamage(10.0f, FDamageEvent(), GetController(), this);
        }
    }
}

float AMyCppCharacter::TakeDamage(float InDamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    OnTakeDamage(InDamageAmount, EventInstigator, DamageCauser);
    return InDamageAmount;
}

void AMyCppCharacter::OnTakeDamage(float InDamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
    CurrentHealth -= InDamageAmount;
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red,
        FString::Printf(TEXT("Health: %.0f / %.0f"), CurrentHealth, MaxHealth));

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AMyCppCharacter::Die()
{
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("You died!"));

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        PC->DisableInput(PC);
        PC->bShowMouseCursor = false;
    }

    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(2.0f);
}