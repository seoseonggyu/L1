// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "Interaction/L1Interactable.h"
#include "Teams/LyraTeamAgentInterface.h"
#include "Protocol.pb.h"
#include "L1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "LyraCharacter.generated.h"

class AActor;
class AController;
class ALyraPlayerController;
class ALyraPlayerState;
class UAbilitySystemComponent;
class UInputComponent;
class ULyraAbilitySystemComponent;
class ULyraCameraComponent;
class ULyraHealthComponent;
class ULyraPawnExtensionComponent;
class UWidgetComponent;
class UObject;

struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;


/**
 * ALyraCharacter
 *
 *	The base character pawn class used by this project.
 *	Responsible for sending events to pawn components.
 *	New behavior should be added via pawn components when possible.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class L1GAME_API ALyraCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface, public IL1Interactable
{
	GENERATED_BODY()

public:
	ALyraCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	ALyraPlayerController* GetLyraPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	ALyraPlayerState* GetLyraPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	virtual ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	void ToggleCrouch();

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface
public:
	void SetOverHeadWidget(TSubclassOf<UUserWidget> InWidgetClass);

protected:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "L1|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidgetComponent;

public:
	virtual FL1InteractionInfo GetPreInteractionInfo(const FL1InteractionQuery& InteractionQuery) const override;
	virtual bool CanInteraction(const FL1InteractionQuery& InteractionQuery) const override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FL1InteractionInfo InteractionInfo;

public:
	float BaseUnscaledCapsuleHalfHeight = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float BaseFallDamage = 3.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxFallDamage = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float FallDamageVelocityZThreshold = 800.f;

public:
	void SetDestInfo(const Protocol::PosInfo& InDestInfo);
	void SetDestInfo(const FVector& InDestInfo);
	void SetVitalInfo(const Protocol::VitalInfo& InVitalInfo);
	void SetStatInfo(const Protocol::StatInfo& InStatInfo);

public:
	int32 GetPlayerId() { return DestInfo->object_id(); }

public:
	Protocol::PosInfo* DestInfo;
	Protocol::VitalInfo* VitalInfo;
	Protocol::StatInfo* StatInfo;

	UPROPERTY()
	ECharacterClassType CharacterClassType = ECharacterClassType::Count;

	FLyraAbilitySet_GrantedHandles AbilitySetGrantedHandles;
};