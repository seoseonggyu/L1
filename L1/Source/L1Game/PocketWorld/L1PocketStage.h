﻿#pragma once

#include "L1PocketStage.generated.h"

class FViewport;
class UCameraComponent;
class UPocketCapture;

UCLASS(BlueprintType)
class AL1PocketStage : public AActor
{
	GENERATED_BODY()
	
public:
	AL1PocketStage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void RefreshLightingChannelToActors();

private:
	void RefreshRenderTargetSize(FViewport* InViewport, uint32 InValue);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPocketCapture* GetPocketCapture() const { return PocketCapture; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACharacter* GetCharacter() const { return SpawnedCharacter; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1CosmeticManagerComponent* GetCosmeticManager() const;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPocketCapture> PocketCaptureClass;
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> SpawnedCharacter;

	UPROPERTY()
	TObjectPtr<UPocketCapture> PocketCapture;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootSceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> CharacterSpawnPoint;

private:
	FDelegateHandle ViewportResizedDelegateHandle;
};
