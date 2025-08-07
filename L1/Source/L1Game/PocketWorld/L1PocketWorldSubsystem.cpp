#include "L1PocketWorldSubsystem.h"

#include "L1PocketStage.h"
#include "PocketLevel.h"
#include "PocketLevelInstance.h"
#include "PocketLevelSystem.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1PocketWorldSubsystem)

UPocketLevelInstance* UL1PocketWorldSubsystem::GetOrCreatePocketLevelFor(ULocalPlayer* LocalPlayer)
{
	UPocketLevelSubsystem* PocketLevelSubsystem = GetWorld()->GetSubsystem<UPocketLevelSubsystem>();
	
	UPocketLevel* PocketLevel = ULyraAssetManager::GetAssetByName<UPocketLevel>("PocketLevelData");
	FVector PocketLevelLocation = FVector(0, 0, -10000.f);

	return PocketLevelSubsystem->GetOrCreatePocketLevelFor(LocalPlayer, PocketLevel, PocketLevelLocation);
}

void UL1PocketWorldSubsystem::RegisterAndCallForGetPocketStage(ULocalPlayer* LocalPlayer, FGetPocketStageDelegate Delegate)
{
	if (Delegate.IsBound() == false)
		return;

	if (CachedPocketStage.IsValid())
	{
		Delegate.Execute(CachedPocketStage.Get());
	}
	else
	{
		if (UPocketLevelInstance* PocketLevelInstance = GetOrCreatePocketLevelFor(LocalPlayer))
		{
			PocketLevelInstance->AddReadyCallback(FPocketLevelInstanceEvent::FDelegate::CreateLambda(
				[Delegate = MoveTemp(Delegate), this](UPocketLevelInstance* Instance)
				{
					Delegate.Execute(CachedPocketStage.Get());
				})
			);
		}
	}
}

void UL1PocketWorldSubsystem::SetPocketStage(AL1PocketStage* InPocketStage)
{
	CachedPocketStage = InPocketStage;
}
