#pragma once

#include "L1PocketWorldSubsystem.generated.h"

class AL1PocketStage;
class UPocketLevelInstance;

DECLARE_DELEGATE_OneParam(FGetPocketStageDelegate, AL1PocketStage*);

UCLASS()
class UL1PocketWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UPocketLevelInstance* GetOrCreatePocketLevelFor(ULocalPlayer* LocalPlayer);
	
public:
	void RegisterAndCallForGetPocketStage(ULocalPlayer* LocalPlayer, FGetPocketStageDelegate Delegate);
	void SetPocketStage(AL1PocketStage* InPocketStage);
	
private:
	UPROPERTY()
	TWeakObjectPtr<AL1PocketStage> CachedPocketStage;
};
